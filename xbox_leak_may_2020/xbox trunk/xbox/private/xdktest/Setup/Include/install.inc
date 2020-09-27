'$include 'winapi.inc'
'$resource 'install.vtr'
' File name: install.inc
'
' Author: Winthrop Chan (winthc@microsoft.com)
' Date  : March 12, 2001
'
' Description:
'	Shared routines for performing install and uninstall automation for the Xbox SDK
'

'
' @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
'
' Global variables and constants
'
' @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
'

'
' Global constants used by Automation
'
Global BuildNumber                as string
Global DefaultTimeOut             as integer
Global UninstallKey               as string
Global TempLocalInstallSourcePath as string
Global TempLocalInstallTargetPath as string
Global TempNetInstallSourcePath   as string
Global TempNetInstallTargetPath   as string
Global TempStartMenuPath          as string
Global DefaultInstallSourceRoot   as string
Global DefaultInstallTargetPath   as string
Global DefaultStartMenuPath       as string

Global CurrentWindow              as Long
Global TimeOut                    as Long
Global ReleaseNotes               as integer

Const  iniFile                    = "c:\XDKSetupTest.ini"
DefaultInstallSourceRoot          = "\\xbuilds\release\usa"
'DefaultInstallSourceRoot          = "\\xbuilds\pvtrelease\usa"
'
' Constants for Find Window flags
'
Const EXIST     = FW_FOCUS or FW_PART or FW_ERROR or FW_ALL
Const NO_EXIST  = FW_NOEXIST or FW_PART or FW_ERROR or FW_ALL
Const MAY_EXIST = FW_FOCUS or FW_PART or FW_ALL
Const FW_BASE_FLAGS = FW_FOCUS Or FW_IGNOREFILE Or FW_ERROR
Const FINDWINDOWFLAGS = FW_BASE_FLAGS

'
' Function definitions for localization support
'
declare function GetSystemDefaultLangID      lib "kernel32.dll" alias "GetSystemDefaultLangID"     () as long
declare function GetSystemDefaultUILanguage  lib "kernel32.dll" alias "GetSystemDefaultUILanguage" () as long


'
' @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
'
' Initialization
'
' @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
'
LoadSettings
'
' Workaround for bug when release notes are missing from build
'
ReleaseNotes = true


'
' Localization Support Strings
'
Global strInstallShield_Wizard$
Global strConfirm_File_Deletion$
Global strMaintenance_Complete$
Global strMicrosoft_Xbox_SDK_Setup$
Global strWelcome_to_Microsoft_Xbox_SDK_Setup$
Global strSevere$
Global strIn_Use$
Global strXbox_SDK_Setup_Notes$
Global strChoose_Destination_Location$
Global strPlease_choose_the_installation_folder$
Global strSelect_Program_Folder$
Global strStart_Copying_Files$
Global strSetup_Status$
Global strXDK_Release_Notes$
Global strXDK_Update_Instructions$
Global strExit_Setup$
Global strWarning$
Global strSetup_Needs_The_Next_Disk$
Global strInstallShield_Wizard_Complete$

'
' Select correct strings for locale
'
if GetSystemDefaultLangID() <> 1041 then	' English Localized strings
	strInstallShield_Wizard  = "InstallShield Wizard"
	strConfirm_File_Deletion = "Confirm File Deletion"
	strMaintenance_Complete  = "Maintenance Complete"
	strMicrosoft_Xbox_SDK_Setup = "Microsoft Xbox SDK Setup"
	strWelcome_to_Microsoft_Xbox_SDK_Setup = "Welcome to Microsoft Xbox SDK Setup"
	strSevere = "Severe"
	strIn_Use = "One or more Visual C++ files is in use. Please close and try again."
	strXbox_SDK_Setup_Notes = "Xbox SDK Setup Notes"
	strChoose_Destination_Location = "Choose Destination Location"
	strPlease_choose_the_installation_folder = "Please choose the installation folder."
	strSelect_Program_Folder = "Select Program Folder"
	strStart_Copying_Files = "Start Copying Files"
	strSetup_Status = "Setup Status"
'	strXDK_Release_Notes = "XDK Release Notes"
'	strXDK_Update_Instructions = "XDK Update Instructions"
	strXDK_Release_Notes = "XDK "
	strXDK_Update_Instructions = "XDK "
	strExit_Setup = "Exit Setup"
	strWarning = "Warning"
	strSetup_Needs_The_Next_Disk = "Setup Needs The Next Disk"
	strInstallShield_Wizard_Complete = "InstallShield Wizard Complete"
else	' Japanese Localized strings
	strInstallShield_Wizard  = "InstallShield ｳｨｻﾞｰﾄﾞ"
	strConfirm_File_Deletion = "ﾌｧｲﾙ削除の確認"
	strMaintenance_Complete  = "ﾒﾝﾃﾅﾝｽの完了"
	strMicrosoft_Xbox_SDK_Setup = "Microsoft Xbox SDK セットアップ"
	strWelcome_to_Microsoft_Xbox_SDK_Setup = "Microsoft Xbox SDK セットアップへようこそ"+chr$(13)
	strSevere = "問題"
	strIn_Use = "ひとつまたは複数の Visual C++ ファイルが使用されています。使用されているのファイルを閉じて、もう一度試してください。"
	strXbox_SDK_Setup_Notes = "Xbox SDK セットアップ ノート"
	strChoose_Destination_Location = "ｲﾝｽﾄｰﾙ先の選択"
	strPlease_choose_the_installation_folder = "ｲﾝｽﾄｰﾙ先のﾌｫﾙﾀﾞを選択してください。"
	strSelect_Program_Folder = "ﾌﾟﾛｸﾞﾗﾑ ﾌｫﾙﾀﾞの選択"
	strStart_Copying_Files = "ﾌｧｲﾙ ｺﾋﾟｰの開始"
	strSetup_Status = "ｾｯﾄｱｯﾌﾟ ｽﾃｰﾀｽ"
'	strXDK_Release_Notes = "XDK リリース ノート"
'	strXDK_Update_Instructions = "XDK アップデートに関する説明"
	strXDK_Release_Notes = "XDK "
	strXDK_Update_Instructions = "XDK "
	strExit_Setup = "ｾｯﾄｱｯﾌﾟの中止"
	strWarning = "警告"
	strSetup_Needs_The_Next_Disk = "次のﾃﾞｨｽｸの挿入"
	strInstallShield_Wizard_Complete = "InstallShield Wizard Complete"
end if




'
' @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
'
' Function definition section
'
' @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
'


'
' Function:    LoadSettings()
'
' Parameters:  none
' Returns:     Nothing
'
' Description: Retrieves Setup Automation settings from INI file
'
' Note:        Only tested with InstallShield based setup Build 3406 +
'
sub LoadSettings()
	Dim strBuffer$
	Dim retVal%


	'
	' BuildNumber
	'
	strBuffer = space(2048)
	retVal = GetPrivateProfileString("Setup Automation Settings", "BuildNumber", "0", strBuffer, len(strBuffer), iniFile)
	if BuildNumber = "0" then
		BuildNumber = LatestBuild
	else
		BuildNumber = trim$(strBuffer)
	end if
	'
	' DefaultTimeOut
	'
	strBuffer = space(2048)
	TimeOut = GetPrivateProfileInt("Setup Automation Settings","DefaultTimeOut", 5, iniFile)
	'
	' UninstallKey
	'
	strBuffer = space(2048)
	retVal = GetPrivateProfileString("Setup Automation Settings","UninstallKey","{958CD46E-504C-4D7B-8336-EA13AAE02B15}", strBuffer, len(strBuffer), iniFile)
	UninstallKey = left$(strBuffer, retVal)
	'
	' TempLocalInstallSourcePath
	'
	strBuffer = space(2048)
	retVal = GetPrivateProfileString("Setup Automation Settings","TempLocalInstallSourcePath","c:\$$Setup$.$$$", strBuffer, len(strBuffer), iniFile)
	TempLocalInstallSourcePath = left$(strBuffer, retVal)
	'
	' TempLocalInstallTargetPath
	'
	strBuffer = space(2048)
	retVal = GetPrivateProfileString("Setup Automation Settings","TempLocalInstallTargetPath","c:\$$Xbox$$.$$$", strBuffer, len(strBuffer), iniFile)
	TempLocalInstallTargetPath = left$(strBuffer, retVal)
	'
	' TempNetInstallSourcePath
	'
	strBuffer = space(2048)
	retVal = GetPrivateProfileString("Setup Automation Settings","TempNetInstallSourcePath","\\xdk\XDKTest\SetupScratch\Setup", strBuffer, len(strBuffer), iniFile)
	TempNetInstallSourcePath = left$(strBuffer, retVal)
	'
	' TempNetInstallTargetPath
	'
	strBuffer = space(2048)
	retVal = GetPrivateProfileString("Setup Automation Settings","TempNetInstallTargetPath","\\xdk\XDKTest\SetupScratch\Target", strBuffer, len(strBuffer), iniFile)
	TempNetInstallTargetPath = left$(strBuffer, retVal)
	'
	' TempStartMenuPath
	'
	strBuffer = space(2048)
	retVal = GetPrivateProfileString("Setup Automation Settings","TempStartMenuPath","Accessories\MS Xbox SDK", strBuffer, len(strBuffer), iniFile)
	TempStartMenuPath = left$(strBuffer, retVal)
	'
	' DefaultInstallSourceRoot
	'
	strBuffer = space(2048)
	retVal = GetPrivateProfileString("Setup Automation Settings","DefaultInstallSourceRoot","\\xbuilds\release\usa", strBuffer, len(strBuffer), iniFile)
	DefaultInstallSourceRoot = left$(strBuffer, retVal)
	'
	' DefaultInstallTargetPath
	'
	strBuffer = space(2048)
	retVal = GetPrivateProfileString("Setup Automation Settings","DefaultInstallTargetPath","c:\Program Files\Microsoft Xbox SDK", strBuffer, len(strBuffer), iniFile)
	DefaultInstallTargetPath = left$(strBuffer, retVal)
	'
	' DefaultStartMenuPath
	'
	strBuffer = space(2048)
	retVal = GetPrivateProfileString("Setup Automation Settings","DefaultStartMenuPath","Microsoft Xbox SDK", strBuffer, len(strBuffer), iniFile)
	DefaultStartMenuPath = left$(strBuffer, retVal)
	'
	' If no INI file exists, then Save settings
	'
	if not exists(iniFile) then
		SaveSettings(false)	
	end if
end sub


'
' Function:    SaveSettings()
'
' Parameters:  useDefaults - Boolean. True if use default values. False if save existing values
' Returns:     Nothing
'
' Description: Saves Setup Automation settings to INI file
'
' Note:        Only tested with InstallShield based setup Build 3406 +
'
sub SaveSettings(useDefaults%=true)
	Dim locBuildNumber$, locDefaultTimeOut$, locUninstallKey$
	Dim locTempLocalInstallSourcePath$, locTempLocalInstallTargetPath$
	Dim locTempNetInstallSourcePath$, locTempNetInstallTargetPath$
	Dim locTempStartMenuPath$, locDefaultInstallSourceRoot$
	Dim locDefaultInstallTargetPath$, locDefaultStartMenuPath$

	if useDefaults then
		locBuildNumber                = LatestBuild()
		locDefaultTimeOut             = "5"
		locUninstallKey               = "{958CD46E-504C-4D7B-8336-EA13AAE02B15}"
		locTempLocalInstallSourcePath = "c:\$$Setup$.$$$"
		locTempLocalInstallTargetPath = "c:\$$Xbox$$.$$$"
		locTempNetInstallSourcePath   = "\\xdk\XDKTest\SetupScratch\Setup"
		locTempNetInstallTargetPath   = "\\xdk\XDKTest\SetupScratch\Target"
		locTempStartMenuPath          = "Accessories\MS Xbox SDK"
		locDefaultInstallSourceRoot   = "\\xbuilds\release\usa"
		locDefaultInstallTargetPath   = "c:\Program Files\Microsoft Xbox SDK"
		locDefaultStartMenuPath       = "Microsoft Xbox SDK"
	else
		locBuildNumber                = trim(BuildNumber)
		locDefaultTimeOut             = trim(str(TimeOut))
		locUninstallKey               = UninstallKey
		locTempLocalInstallSourcePath = TempLocalInstallSourcePath
		locTempLocalInstallTargetPath = TempLocalInstallTargetPath
		locTempNetInstallSourcePath   = TempNetInstallSourcePath
		locTempNetInstallTargetPath   = TempNetInstallTargetPath
		locTempStartMenuPath          = TempStartMenuPath
		locDefaultInstallSourceRoot   = DefaultInstallSourceRoot
		locDefaultInstallTargetPath   = DefaultInstallTargetPath
		locDefaultStartMenuPath       = DefaultStartMenuPath
	end if

	'
	' BuildNumber
	'
	WritePrivateProfileString("Setup Automation Settings","BuildNumber", locBuildNumber, iniFile)
	'
	' DefaultTimeOut
	'
	WritePrivateProfileString("Setup Automation Settings","DefaultTimeOut", locDefaultTimeOut, iniFile)
	'
	' UninstallKey
	'
	WritePrivateProfileString("Setup Automation Settings","UninstallKey", locUninstallKey, iniFile)
	'
	' TempLocalInstallSourcePath
	'
	WritePrivateProfileString("Setup Automation Settings","TempLocalInstallSourcePath", locTempLocalInstallSourcePath, iniFile)
	'
	' TempLocalInstallTargetPath
	'
	WritePrivateProfileString("Setup Automation Settings","TempLocalInstallTargetPath", locTempLocalInstallTargetPath, iniFile)
	'
	' TempNetInstallSourcePath
	'
	WritePrivateProfileString("Setup Automation Settings","TempNetInstallSourcePath", locTempNetInstallSourcePath, iniFile)
	'
	' TempNetInstallTargetPath
	'
	WritePrivateProfileString("Setup Automation Settings","TempNetInstallTargetPath", locTempNetInstallTargetPath, iniFile)
	'
	' TempStartMenuPath
	'
	WritePrivateProfileString("Setup Automation Settings","TempStartMenuPath", locTempStartMenuPath, iniFile)
	'
	' DefaultInstallSourceRoot
	'
	WritePrivateProfileString("Setup Automation Settings","DefaultInstallSourceRoot", locDefaultInstallSourceRoot, iniFile)
	'
	' DefaultInstallTargetPath
	'
	WritePrivateProfileString("Setup Automation Settings","DefaultInstallTargetPath", locDefaultInstallTargetPath, iniFile)
	'
	' DefaultStartMenuPath
	'
	WritePrivateProfileString("Setup Automation Settings","DefaultStartMenuPath", locDefaultStartMenuPath, iniFile)
end sub

'
' Function:    Init()
'
' Parameters:  none
' Returns:     Nothing
'
' Description: Opens Settings Dialog to set Automation settings
'
sub Init()
	dim Dlg as IDD_SETTINGS

	viewport clear

	LoadSettings()

	dlg.Proc = varptr(SettingsDlgProc)

	dlg.IDC_BUILDNUM         = trim$(BuildNumber)
	dlg.IDC_TIMEOUT          = trim(str(TimeOut))
	dlg.IDC_UNINSTALLKEY     = UninstallKey
	dlg.IDC_DEFAULTSOURCE    = DefaultInstallSourceRoot
	dlg.IDC_DEFAULTTARGET    = DefaultInstallTargetPath
	dlg.IDC_DEFAULTSTARTMENU = DefaultStartMenuPath
	dlg.IDC_TEMPSTARTMENU    = TempStartMenuPath
	dlg.IDC_LOCALSOURCE      = TempLocalInstallSourcePath
	dlg.IDC_NETWORKSOURCE    = TempNetInstallSourcePath
	dlg.IDC_LOCALTARGET      = TempLocalInstallTargetPath
	dlg.IDC_NETWORKTARGET    = TempNetInstallTargetPath

	dialog(dlg)
	SaveSettings(false)
end sub


'
' Function:    SettingsDlgProc()
'
' Parameters:  ...
' Returns:     long
'
' Description: Settings Dialog Command procedure
'
Function SettingsDlgProc(hwnd&, msg&, wParam&, lParam&) as long
	SettingsDlgProc = TRUE
	select case msg
		Case WM_COMMAND
			Select Case wParam
				Case IDOK
					BuildNumber                = trim$(edittext("#"+trim$(str$(IDC_BUILDNUM))))
					TimeOut                    = val(edittext("#"+trim$(str$(IDC_TIMEOUT))))
					UninstallKey               = trim$(edittext("#"+trim$(str$(IDC_UNINSTALLKEY))))
					DefaultInstallSourceRoot   = trim$(edittext("#"+trim$(str$(IDC_DEFAULTSOURCE))))
					DefaultInstallTargetPath   = trim$(edittext("#"+trim$(str$(IDC_DEFAULTTARGET))))
					DefaultStartMenuPath       = trim$(edittext("#"+trim$(str$(IDC_DEFAULTSTARTMENU))))
					TempStartMenuPath          = trim$(edittext("#"+trim$(str$(IDC_TEMPSTARTMENU))))
					TempLocalInstallSourcePath = trim$(edittext("#"+trim$(str$(IDC_LOCALSOURCE))))
					TempNetInstallSourcePath   = trim$(edittext("#"+trim$(str$(IDC_NETWORKSOURCE))))
					TempLocalInstallTargetPath = trim$(edittext("#"+trim$(str$(IDC_LOCALTARGET))))
					TempNetInstallTargetPath   = trim$(edittext("#"+trim$(str$(IDC_NETWORKTARGET))))

					SettingsDlgProc = FALSE
				Case IDC_DEFAULTS
					wEditSetText("#"+trim$(str$(IDC_BUILDNUM)),         LatestBuild())
					wEditSetText("#"+trim$(str$(IDC_TIMEOUT)),          "5")
					wEditSetText("#"+trim$(str$(IDC_UNINSTALLKEY)),     "{958CD46E-504C-4D7B-8336-EA13AAE02B15}")
					wEditSetText("#"+trim$(str$(IDC_DEFAULTSOURCE)),    "\\xbuilds\release\usa")
					wEditSetText("#"+trim$(str$(IDC_DEFAULTTARGET)),    "c:\Program Files\Microsoft Xbox SDK")
					wEditSetText("#"+trim$(str$(IDC_DEFAULTSTARTMENU)), "Microsoft Xbox SDK")
					wEditSetText("#"+trim$(str$(IDC_TEMPSTARTMENU)),    "Accessories\MS Xbox SDK")
					wEditSetText("#"+trim$(str$(IDC_LOCALSOURCE)),      "c:\$$Setup$.$$$")
					wEditSetText("#"+trim$(str$(IDC_NETWORKSOURCE)),    "\\xdk\XDKTest\SetupScratch\Setup")
					wEditSetText("#"+trim$(str$(IDC_LOCALTARGET)),      "c:\$$Xbox$$.$$$")
					wEditSetText("#"+trim$(str$(IDC_NETWORKTARGET)),    "\\xdk\XDKTest\SetupScratch\Target")
			End select
		case else
			SettingsDlgProc = FALSE
	end select
end function


'
' Function:    HitKey()
'
' Parameters:  CurDialog%		- Current Dialog of setup
'              buttonID%		- Array of button ID's
'              buttonKey%		- Array of button Keystrokes
'              CancelAt%		- Install dialog to call cancel at
'									0 - Updater Start Dialog
'									1 - Updater Extracting setup dialog
'									2 - Main setup welcome screen
'									3 - Setup notes screen
'									4 - Destination Location screen
'									5 - Program Folder Screen
'									6 - Start Copying Files screen
'									7 - Setup Status Screen
' Returns:     Nothing
'
' Description: Helper function to call either the button click or hotkey
'
' Note:        Only tested with InstallShield based setup Build 3406 +
'
Sub HitKey(CurDialog%, buttonID$(), buttonKey$(), UseHotKeys%=false, CancelAt%=-1)
	Dim hitType%

	if CurDialog = CancelAt then
		hitType = 1
	else
		hitType = 0
	End if

	'
	' Problem with InstallShield. Need to set focus on a button prior to sendkey
	'
	WButtonSetFocus(buttonID(CurDialog, hitType))
	if UseHotKeys then
		Play buttonKey(CurDialog, hitType)
	else
		WButtonClick(buttonID(CurDialog, hitType), 10)
	end if
end Sub


'
' Function:    IsInstalled()
'
' Parameters:  none
' Returns:     True		- If Xbox SDK is installed
'              False	- If Xbox SDK is not installed
'
' Description: Checks to see if SDK is installed
'
' Note:        Only tested with InstallShield based setup Build 3406 +
'
function IsInstalled() as short
	IsInstalled = RegistryKeyExists("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\"+UninstallKey)
end function


'
' Function:    LatestBuild()
'
' Parameters:  none
' Returns:     String	- If a build number exists
'              NULL		- If unable to determine build
'
' Description: Returns latest build on network drop
'
' Note:        Only tested with InstallShield based setup Build 3406 +
'
function LatestBuild() as string
	Dim tempBuild$
	Dim intFile as integer

	'
	' Get latest Build Number
	'
	if not exists(DefaultInstallSourceRoot+"\latest.txt") then
		LatestBuild = "0"
	else
		intfile = FREEFILE
		open DefaultInstallSourceRoot+"\latest.txt" for input as intfile
		line input #intFile, tempBuild
		close #1
		LatestBuild = trim$(tempBuild)
	end if
end function


'
' Function:    GetXDKPath()
'
' Parameters:  none
' Returns:     String	- path to XDK Installation if it exists
'
' Description: returns path to XDK Installation if it exists
'
' Note:        Only tested with InstallShield based setup Build 3406 +
'
function GetXDKPath() as string
	if RegistryValueExists("HKEY_CURRENT_USER\Environment","XDK") then
		GetXDKPath = RegistryGetValue("HKEY_CURRENT_USER\Environment","XDK")
	else
		GetXDKPath = ""
	endif
end function


'
' Function:    CancelInstall()
'
' Parameters:  UseHotKeys%	- True if using hotkeys. False if using buttons
' Returns:     nothing
'
' Description: Automates the Install process for the SDK
'
' Note:        Only tested with InstallShield based setup Build 3406 +
'
sub CancelInstall(UseHotKeys%=false)
	CurrentWindow = WFndWndC(strExit_Setup, "#32770", FINDWINDOWFLAGS, 15)
	if UseHotKeys then
		Play "%(y)"
	else
		WButtonSetFocus("#6")
		WButtonClick("#6", 10)
	end if
	sleep 5
end sub


'
' Function:    Install()
'
' Parameters:  BuildNum%		- Build Number to install
'              UseUpdater%		- True to use the updater setup and false to run the full SDK setup
'              UseHotKeys%		- True to use hotkeys and false to use button clicks
'              CancelAt%		- Install dialog to call cancel at
'									0 - Updater Start Dialog
'									1 - Updater Extracting setup dialog
'									2 - Main setup welcome screen
'									3 - Setup notes screen
'									4 - Destination Location screen
'									5 - Program Folder Screen
'									6 - Start Copying Files screen
'									7 - Setup Status Screen
'              InstallSource$	- Path to executable. If BuildNum is 0, then this is the full pathname to run
'              InstallPath$		- Path to install SDK
'              StartMenuPath$	- Start Menu path
'              RelNoteTitle$    - Window title for release notes
' Returns:     Nothing
'
' Description: Automates the Install process for the SDK
'
' Note:        Only tested with InstallShield based setup Build 3406 +
'
Sub Install(BuildNum$, UseUpdater%, UseHotKeys%=false, CancelAt%=-1, InstallSource$="", InstallPath$="", StartMenuPath$="", RelNoteTitle$="")
	Dim strSetupExeFile As String
	Dim intBuild As long
	Dim IsUpdaterSetup as integer
	Dim hwnd as long
	Dim details as INFO
	Dim buttonID$(15,2)
	Dim buttonKey$(15,2)
	Dim IsUpdate as integer
	Dim FinishString$
	Dim InstalledBuild$
	Dim IsQFE as integer
	Dim BuildKey$


	'
	' Special condition when running the setup on an existing installation
	'
	if RegistryKeyExists("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\XboxSDK") then
		if RegistryValueExists("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\XboxSDK", "July01SDK") then
			BuildKey = "July01SDK"
		elseif RegistryValueExists("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\XboxSDK", "June01SDK") then
			BuildKey = "June01SDK"
		elseif RegistryValueExists("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\XboxSDK", "May01SDK") then
			BuildKey = "May01SDK"
		elseif RegistryValueExists("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\XboxSDK", "April01SDK") then
			BuildKey = "April01SDK"
		elseif RegistryValueExists("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\XboxSDK", "March01SDK") then
			BuildKey = "March01SDK"
		elseif RegistryValueExists("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\XboxSDK", "Build") then
			BuildKey = "Build"
		else
			BuildKey = ""
		end if
		if BuildKey <> "" then
			InstalledBuild = trim$(str$(RegistryGetValue("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\XboxSDK", BuildKey)))
		else
			InstalledBuild = "0"
		end if
	else
		InstalledBuild = "0"
	end if
		
	if IsInstalled and (InstalledBuild < BuildNum) then
		IsUpdate = true
		FinishString = strMaintenance_Complete
		if instr(BuildNum, ".") <> 0 then
			IsQFE = true
			if BuildNum > "3147" then
				FinishString = 	strInstallShield_Wizard_Complete
			end if
		end if
	else
		IsUpdate = false
		FinishString = strMicrosoft_Xbox_SDK_Setup
	end if

	'
	' Initialize Keystrokes
	'
	' Updater Start Dialog
	buttonID ( 0,0) = _id(&h3024)
	buttonKey( 0,0) = "%(n)"
	buttonID ( 0,1) = _id(2)
	buttonKey( 0,1) = "{ESC}"
	' Updater Extracting Setup Dialog
	buttonID ( 1,0) = ""
	buttonKey( 1,0) = ""
	buttonID ( 1,1) = _id(2)
	buttonKey( 1,1) = " "
	' Main Setup Welcome
	buttonID ( 2,0) = _id(1)
	buttonKey( 2,0) = "%(n)"
	buttonID ( 2,1) = _id(9)
	buttonKey( 2,1) = "{ESC}"
	' Setup Notes
	buttonID ( 3,0) = _id(6)
	buttonKey( 3,0) = "%(y)"
	buttonID ( 3,1) = _id(9)
	buttonKey( 3,1) = "%(n)"
	' Destination Location
	buttonID ( 4,0) = _id(1)
	buttonKey( 4,0) = "%(n)"
	buttonID ( 4,1) = _id(9)
	buttonKey( 4,1) = " "
	' Program Folder
	buttonID ( 5,0) = _id(1)
	buttonKey( 5,0) = "%(n)"
	buttonID ( 5,1) = _id(9)
	buttonKey( 5,1) = " "
	' Start Copying Files
	buttonID ( 6,0) = _id(1)
	buttonKey( 6,0) = "%(n)"
	buttonID ( 6,1) = _id(9)
	buttonKey( 6,1) = " "
	' Setup Status
	buttonID ( 7,0) = ""
	buttonKey( 7,0) = ""
	buttonID ( 7,1) = _id(2)
	buttonKey( 7,1) = " "
	' Setup Complete
	buttonID ( 8,0) = _id(1)
	buttonKey( 8,0) = "{ENTER}"
	buttonID ( 8,1) = ""
	buttonKey( 8,1) = ""


	if trim$(InstallSource) = "" then
		strSetupExeFile = DefaultInstallSourceRoot + "\" + trim$(BuildNum) + "\"
	else
		if lcase(right(trim$(InstallSource),4)) <> ".exe" then
			strSetupExeFile = InstallSource + "\"
		else
			strSetupExeFile = InstallSource
		endif
	end if

	if strSetupExeFile <> InstallSource then
		if UseUpdater then
			strSetupExeFile = strSetupExeFile + "XboxSDKUpdate" + trim$(BuildNum) + ".exe"
		else
			strSetupExeFile = strSetupExeFile + "sdk\setup.exe"
		end if
	end if

	LOG #10, "Executing Installer: "+strSetupExeFile
	if exists(strSetupExeFile) then
		Run strSetupExeFile, NOWAIT
	else
		FAIL "Install Program does not exist: "+strSetupExeFile
	end if

	if UseUpdater AND not IsQFE then
		'
		' Initial Updater dialog
		'
		CurrentWindow = WFndWndC("Microsoft Xbox SDK Update - InstallShield Wizard", "#32770", FINDWINDOWFLAGS, 300)
		HitKey(0, buttonID, buttonKey, UseHotKeys, CancelAt)
		if (CancelAt = 0) then
			sleep 5
			goto installverify
		end if

		'
		' Expand setup files dialog
		'
		hwnd = WButtonFind(_id(2), 10)
		if hwnd <> 0 and CancelAt = 1 then
			sleep 2
			HitKey(1, buttonID, buttonKey, UseHotKeys, CancelAt)
			CurrentWindow = WFndWndC("Microsoft Xbox SDK Update", "#32770", FINDWINDOWFLAGS, 15)
			if UseHotKeys then
				Play "%(y)"
			else
				WButtonSetFocus("#6")
				WButtonClick("#6", 10)
			end if
			goto installverify
		end if
	end if

	'
	' Setup Welcome Dialog
	'
	CurrentWindow = WFndWndC(strMicrosoft_Xbox_SDK_Setup, "#32770", MAY_EXIST or FW_NOERROR, 240)
	if CurrentWindow = 0 then
		hwnd = WFndWndC(strSevere, "#32770", EXIST, 15)
		if hwnd <> 0 then
			hwnd = WStaticFind(strIn_Use, 5)
			if hwnd <> 0 then WButtonClick("#2")
			FAIL "An Instance of VC was running during the install"
		end if
	end if
	hwnd = WStaticFind(strWelcome_to_Microsoft_Xbox_SDK_Setup, 5)
	if hwnd = 0 then
		FAIL "Setup should be at '"+strWelcome_to_Microsoft_Xbox_SDK_Setup+"' dialog"
	end if
	HitKey(2, buttonID, buttonKey, UseHotKeys, CancelAt)
	if (CancelAt = 2) then
		CancelInstall(UseHotKeys)
		goto installverify
	end if

'
' Need this since running updater on top of existing install skips these screens
'		
if NOT IsUpdate then
	'
	' Setup Notes
	'
	CurrentWindow = WFndWndC(strMicrosoft_Xbox_SDK_Setup, "#32770", EXIST, 30)
	hwnd = WStaticFind(strXbox_SDK_Setup_Notes, 5)
	if hwnd = 0 then
		FAIL "Setup should be at '"+strXbox_SDK_Setup_Notes+"' dialog"
	end if
	WButtonSetfocus("#9")
	HitKey(3, buttonID, buttonKey, UseHotKeys, CancelAt)
	if (CancelAt = 3) then
		CancelInstall(UseHotKeys)
		goto installverify
	end if

	'
	' Setup Destination Location
	'
	CurrentWindow = WFndWndC(strMicrosoft_Xbox_SDK_Setup, "#32770", EXIST, 30)
	hwnd = WStaticFind(strChoose_Destination_Location, 5)
	if hwnd = 0 then
		FAIL "Setup should be at '"+strChoose_Destination_Location+"' dialog"
	end if
	if InstallPath = "" then
		InstallPath = DefaultInstallTargetPath
	else
		WButtonSetFocus("#9")
		if UseHotKeys then
			Play "%(b)"
'			Play "%(r)"	' Japanese
		else
			WButtonClick("#196")
		end if
		hwnd = WStaticFind(strPlease_choose_the_installation_folder, 5)
		if hwnd = 0 then
			FAIL "Setup should be at '"+strPlease_choose_the_installation_folder+"' dialog"
		end if
		WEditSetText("#4", InstallPath, 30)
		WButtonSetFocus("#1")
		WButtonClick("#1")
		if exists(InstallPath, "+d") = false then
			sleep 1
			WButtonClick("#6")
		end if
	end if
	HitKey(4, buttonID, buttonKey, UseHotKeys, CancelAt)
	if (CancelAt = 4) then
		CancelInstall(UseHotKeys)
		goto installverify
	end if


	'
	' Setup Start Menu Folder
	'
	CurrentWindow = WFndWndC(strMicrosoft_Xbox_SDK_Setup, "#32770", EXIST, 30)
	hwnd = WStaticFind(strSelect_Program_Folder, 5)
	if hwnd = 0 then
		FAIL "Setup should be at '"+strSelect_Program_Folder+"' dialog"
	end if
	if StartMenuPath = "" then
		StartMenuPath = defaultStartMenuPath
	else
		WEditSetText("#301", StartMenuPath, 30)
	end if
	HitKey(5, buttonID, buttonKey, UseHotKeys, CancelAt)
	if (CancelAt = 5) then
		CancelInstall(UseHotKeys)
		goto installverify
	end if

	'
	' Start Copying Files Dialog
	'
	CurrentWindow = WFndWndC(strMicrosoft_Xbox_SDK_Setup, "#32770", EXIST, 30)
	hwnd = WStaticFind(strStart_Copying_Files, 5)
	if hwnd = 0 then
		FAIL "Setup should be at '"+strStart_Copying_Files+"' dialog"
	end if
	HitKey(6, buttonID, buttonKey, UseHotKeys, CancelAt)
	if (CancelAt = 6) then
		CancelInstall(UseHotKeys)
		goto installverify
	end if
end if

if not IsQFE then
	'
	' Setup Status dialog (cancel only)
	'
	CurrentWindow = WFndWndC(strMicrosoft_Xbox_SDK_Setup, "#32770", EXIST, 30)
	hwnd = WStaticFind(strSetup_Status, 5)
	if hwnd = 0 then
		FAIL "Setup should be at '"+strSetup_Status+"' dialog"
	end if
	if CancelAt = 7 then
		sleep 5
		HitKey(7, buttonID, buttonKey, UseHotKeys, CancelAt)
		CancelInstall(UseHotKeys)
		goto installverify
	end if
end if

	'
	' Setup Complete Dialog
	'
	CurrentWindow = WFndWndC(strMicrosoft_Xbox_SDK_Setup, "#32770", EXIST, 30)
	hwnd = WStaticFind(FinishString, 480)
	if hwnd = 0 then
		FAIL "Setup should be at '"+FinishString+"' dialog"
	end if
	if IsQFE and BuildNum > "3147" then
		WOptionSetFocus("#503")
		WOptionClick("#503", 10)
	end if
	HitKey(8, buttonID, buttonKey, UseHotKeys, CancelAt)

	'
	' Release notes
	'
	if RelNoteTitle = "" then
		if IsUpdate then
			RelNoteTitle = strXDK_Update_Instructions
		else
			RelNoteTitle = strXDK_Release_Notes
		end if
	end if
	CurrentWindow = WFndWnd(RelNoteTitle, MAY_EXIST or FW_NOERROR, 30)
	if CurrentWindow = 0 then
		if ReleaseNotes then
			LOG #1, "Setup should have launched release notes in browser"
		end if
	else
		Play "%{F4}"
'		Play "%(f)"
'		WMenuSelect("&Close")
	end if

	'
	' Verify Microsoft Xbox SDK Setup is shutdown
	'
installverify:
	CurrentWindow = WFndWndC(strMicrosoft_Xbox_SDK_Setup, "#32770", NO_EXIST, 30)
	Sleep 5
End Sub


'
' Function:    Uninstall()
'
' Parameters:  None
' Returns:     Nothing
'
' Description: Calls uninstall procedure as stored in the registry for the Xbox SDK Setup
'
' Note:        Only tested with InstallShield based setup Build 3407
'
Sub Uninstall()
	Dim uninstallApp As String

	' Used for storing the return values for WINAPI calls

	uninstallApp = NULL

	' Initilize values for use
	uninstallApp = SPACE$(1024)

	uninstallApp = trim$(RegistryGetValue("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\"+UninstallKey, "UninstallString"))

	If uninstallApp <> "" Then
		'Start the uninstall program
		Run uninstallApp, NOWAIT
		'
		' Wait for initial setup screen
		'
		wFndWndC(strInstallShield_Wizard, "#32770", EXIST, 300)
		WButtonSetFocus("#1")
		Play "%(r)"	' select remove
		wButtonClick("#1")
		'
		' Confirm deletion
		'
		wFndWndC(strConfirm_File_Deletion, "#32770", EXIST, 30)
		wButtonClick("OK")
		'
		' Make sure dialog is gone
		'
		wFndWndC(strConfirm_File_Deletion, "#32770", NO_EXIST, 5)
		'
		' Dismiss Uninstall program
		'
		wFndWndC(strInstallShield_Wizard, "#32770", EXIST, 300)
		WStaticFind(strMaintenance_Complete, 420)
		WButtonSetFocus("#1")
		wButtonClick("#1")
		'
		' Make sure InstallShield is shutdown
		'
		wFndWndC(strInstallShield_Wizard, "#32770", NO_EXIST, 5)
	EndIf
	'
	' Wait about 10 seconds for InstallShield to properly clean up and shutdown
	'
	Sleep 10
End Sub


'
' Function:    ModifyInstall()
'
' Parameters:  None
' Returns:     Nothing
'
' Description: Calls setup procedure and attempts to modify installation (should always fail)
'
' Note:        Only tested with InstallShield based setup Build 3407
'
sub ModifyInstall()
	Dim uninstallApp As String

	' Used for storing the return values for WINAPI calls

	uninstallApp = NULL

	' Initilize values for use
	uninstallApp = SPACE$(1024)

	uninstallApp = trim$(RegistryGetValue("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\"+UninstallKey, "UninstallString"))

	If uninstallApp <> "" Then

		'Start the uninstall program
		Run uninstallApp, NOWAIT
		'
		' Wait for initial setup screen
		'
		wFndWndC(strInstallShield_Wizard, "#32770", EXIST, 300)
		WButtonSetFocus("#1")
		Play "%(m)"	' select modify
		wButtonClick("#1")
		'
		' Confirm you cannot do this
		'
		wFndWndC(strWarning, "#32770", EXIST, 30)
		wButtonClick("#2")
		'
		' Make sure dialog is gone
		'
		wFndWndC(strWarning, "#32770", NO_EXIST, 5)
		'
		' Dismiss Uninstall program
		'
		wFndWndC(strInstallShield_Wizard, "#32770", EXIST, 30)
		wButtonClick("#9")
		'
		' Dismiss program
		'
		wFndWndC(strExit_Setup, "#32770", EXIST, 300)
		wButtonClick("#6")
		'
		' Make sure InstallShield is shutdown
		'
		wFndWndC(strInstallShield_Wizard, "#32770", NO_EXIST, 5)
	EndIf
	'
	' Wait about 10 seconds for InstallShield to properly clean up and shutdown
	'
	Sleep 10
end sub

'
' Function:    RepairInstall()
'
' Parameters:  None
' Returns:     Nothing
'
' Description: Calls setup procedure and attempts to repair installation
'
' Note:        Only tested with InstallShield based setup Build 3407
'
sub RepairInstall()
	Dim uninstallApp As String
	Dim hwnd as long

	uninstallApp = NULL

	' Initilize values for use
	uninstallApp = SPACE$(1024)

	'See if the app has an UninstallString in the registry.
	'If it does, assume the app is installed and uninstall it.
	uninstallApp = trim$(RegistryGetValue("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\"+UninstallKey, "UninstallString"))

	If uninstallApp <> "" Then

		'Start the uninstall program
		Run uninstallApp, NOWAIT
		'
		' Wait for initial setup screen
		'
		wFndWndC(strInstallShield_Wizard, "#32770", EXIST, 300)
		WButtonSetFocus("#1")
		Play "%(e)"	' select Repair
		wButtonClick("#1")
		'
		' Look for Setup Status and then Finish
		'
		hwnd = wFndWndC(strSetup_Needs_The_Next_Disk, "#32770", MAY_EXIST, 3)
		if hwnd <> 0 then
			wButtonClick("#9")
			'
			' Hit cancel
			'
'			wFndWndC(strInstallShield_Wizard, "#32770", EXIST, 30)
'			WButtonClick("#9")
			CurrentWindow = WFndWndC(strExit_Setup, "#32770", FINDWINDOWFLAGS, 15)
			WButtonSetFocus("#6")
			WButtonClick("#6", 10)
		else
			wFndWndC(strInstallShield_Wizard, "#32770", EXIST, 30)
			hwnd = WStaticFind(strSetup_Status, 5)
			'
			' Dismiss Uninstall program
			'
			wFndWndC(strInstallShield_Wizard, "#32770", EXIST, 30)
			hwnd = WStaticFind(strMaintenance_Complete, 420)
			WButtonSetFocus("#1")
			wButtonClick("#1")
		end if

		'
		' Make sure InstallShield is shutdown
		'
		wFndWndC(strInstallShield_Wizard, "#32770", NO_EXIST, 5)
	EndIf
	'
	' Wait about 10 seconds for InstallShield to properly clean up and shutdown
	'
	Sleep 10
end sub


'
' Function:    KillDir()
'
' Parameters:  strDir$     - Directory to remove
' Returns:     Nothing
'
' Description: Deletes directory and all contents (including sub directories)
'
' Note:        Only tested with InstallShield based setup Build 3407
'
Sub KillDir(strDir$)
	If Not Exists(strDir, "+d") Then Exit Sub 
	run "cmd /a /c rd /s /q """+strDir+"""",, SW_HIDE
End Sub 


'
' Function:    CopyDir()
'
' Parameters:  srcDir$    - Directory to copy from
'              destDir$   - Directory to copy to
' Returns:     Nothing
'
' Description: copies directory and all contents (including sub directories)
'
' Note:        Only tested with InstallShield based setup Build 3407
'
sub CopyDir(srcDir$, destDir$)
	run "cmd /a /c xcopy /e /h /r /k /y /z """+srcDir+"\*.*"" "+destDir
end sub


'
' Function:    VerifyFile()
'
' Parameters:  verifyType%      - VerifyFile type
'                   1 = Reference List
'                   2 = Found in Installation
'                   3 = In Reference but not Installed
'                   4 = Installed, but not in Reference
'                   5 = Mismatched Timestamps/versions
'              FileArray$()     - Returns all the items in the File ListView
'              RegArray$()      - Returns all the items in the Registry ListView
'              RefList$=""      - Alternate Reference File. Default is "", which uses the apps default value
'              SDKPath$=""      - Alternate SDK Path. Default is "", which uses the apps default value
'              VSCommonPath$="" - Alternate VStudio Common Path. Default is "", which uses the apps default value
' Returns:     Nothing
'
' Description: Calls File Verification tool and returns results found in ListViews
'
' Note:        Only tested with InstallShield based setup Build 3407
'
sub VerifyFile(verifyType%=3, FileArray$(), RegArray$(), RefList$="", SDKPath$="", VSCommonPath$="")
	dim FileListView$, RegListView$
	dim actFCol$, actRCol$
	dim count%, item%
	dim hwnd as long

	FileListView = "#1002"
	RegListView  = "#1003"

	'
	' Check to see if tool is available
	'
	if not exists("\\xdk\XDKTest\Tools\File Verification\xdkfiles.exe") then
		FAIL "Unable to find File Verification tool at: \\xdk\XDKTest\Tools\File Verification\xdkfiles.exe"
	end if
	'
	' Kill off other instances of tool first
	'
	hwnd = WFndWndC("XBox SDK Files Checker", "#32770", MAY_EXIST or FW_NOERROR, 5)
	while hwnd <> 0
		WSetActWnd(hwnd)
		WButtonSetFocus("#1011", 30)
		Play "%{f4}"
		hwnd = WFndWndC("XBox SDK Files Checker", "#32770", MAY_EXIST or FW_NOERROR, 5)
	wend

	'
	' Call Tool and set it to active
	'
	run "\\xdk\XDKTest\Tools\File Verification\xdkfiles.exe", NOWAIT
	hwnd = WFndWndC("XBox SDK Files Checker", "#32770", FINDWINDOWFLAGS, 30)

	'
	' Set Reference List path if one was passed and it exists
	' 
	WSetActWnd(hwnd)
	if trim$(RefList) <> "" then
		if not exists(RefList) then
			FAIL "Invalid Reference List passed to VerifyFile()"
			exit sub
		end if
		WEditSetFocus("#1009")
		WEditSetText("#1009", RefList, 30)
	end if
	'
	' Set SDK Installation path if one was passed and it exists
	' 
	if trim$(SDKPath) <> "" then
		if not exists(SDKPath, "+d") then
			FAIL "Invalid SDK Installation Path passed to VerifyFile()"
			exit sub
		end if
		WEditSetFocus("#1012")
		WEditSetText("#1012", SDKPath, 30)
	end if
	'
	' Set VStudio Common path if one was passed and it exists
	' 
	if trim$(VSCommonPath) <> "" then
		if not exists(VSCommonPath, "+d") then
			FAIL "Invalid VC Common Path passed to VerifyFile()"
			exit sub
		end if
		WEditSetFocus("#1016")
		WEditSetText("#1016", VSCommonPath, 30)
	end if

	'
	' Select Verify Type to execute
	'
	WSetActWnd(hwnd)
	WButtonSetFocus("#1011")
	select case verifyType
		case 1	' Reference List
			WOptionClick("#1018", 30)
		case 2	' Found in Installation
			WOptionClick("#1014", 30)
		case 3	' In Reference but not Installed
			WOptionClick("#1013", 30)
		case 4	' Installed, but not in Reference
			WOptionClick("#1015", 30)
		case 5	' Mismatched Timestamps/versions
			WOptionClick("#1021", 30)
		case else
			FAIL "Invalid Verify Type passed to VerifyFile()"
	end select
	'
	' Execute File Verify and wait 30 seconds for completion
	'
	WButtonClick("#1011", 30)	' Check Installation
	sleep 5

	'
	' Set columns to grab depending on type
	'
	if verifyType = 5 then
		actFCol = "@5"
		actRCol = "@3"
	else
		actFCol = "@3"
		actRCol = "@2"
	end if

	'
	' Set Active window to FileVerify tool
	'
	WSetActWnd(hwnd)
	'
	' Grab items from File ListView
	'
	count = WViewCount(FileListView)
	Redim FileArray$(count, 3)
	For item = 0 to count-1
		FileArray(item, 0) = ViewItemText(FileListView,_ord(item+1), "@1")
		FileArray(item, 2) = ViewItemText(FileListView,_ord(item+1), actFCol)
		if verifyType = 5 then
			if FileArray(item, 2) = "" then
				FileArray(item, 1) = ViewItemText(FileListView,_ord(item+1), "@2")
				FileArray(item, 2) = ViewItemText(FileListView,_ord(item+1), "@3")
			else
				FileArray(item, 1) = ViewItemText(FileListView,_ord(item+1), "@4")
			end if
		end if
	Next item
	'
	' Set Active window to FileVerify tool
	'
	WSetActWnd(hwnd)
	'
	' Grab items from Registry ListView
	'
	count = WViewCount(RegListView)
	Redim RegArray$(count, 3)
	For item = 0 to count-1
		RegArray(item, 0) = ViewItemText(RegListView,_ord(item+1), "@1")
		RegArray(item, 2) = ViewItemText(RegListView,_ord(item+1), actRCol)
		if verifyType = 5 then
			RegArray(item, 1) = ViewItemText(RegListView,_ord(item+1), "@2")
		end if
	Next item

	'
	' Shutdown FileVer Tool
	'
	WSetActWnd(hwnd)
	WButtonSetFocus("#1011", 30)
	Play "%{f4}"
	'
	' Make sure it's gone
	'
	CurrentWindow = WFndWndC("XBox SDK Files Checker", "#32770", NO_EXIST, 30)
end sub


'
' Assumes fresh install of samples
' Assumes no DSW file exists
'
function BuildProject(project$, rebuildAll%=false) as integer
	Dim VStudio as variant, prop as variant, doc as variant
	Dim projCol as variant, proj as variant
	Dim numProjects as variant, numConfigs as variant, i as long, j as long
	Dim configCol as variant, config as variant
	Dim projType as variant, numErrors as long
	Dim projName as string
	Dim pass%

	pass = true

	'
	' Make sure project exists
	'
	if not exists(project) then
		LOG #10, "Project file does not exist"
		BuildProject = false
		exit function
	end if
	'
	' Get automation object for Visual Studio
	'
	VStudio = OleCreateObject("MSDev.Application")
	if TypeID(VStudio) = 0 then
		LOG #10, "Unable to retrieve automation object for Visual Studio"
	end if
	'
	' Make Visual Studio visible
	'
	prop = OleSetProperty(VStudio, "Visible", true)
	
	'
	' Get document object for Visual Studio
	'
	doc = OleGetProperty(VStudio, "Documents")
	if TypeID(doc) = 0 then
		LOG #10, "Unable to retrieve document object for Visual Studio"
	end if
	'
	' Close All documents and open the selected project
	'
	dim ret as variant
	CurrentWindow = Wfndwnd("Visual C++", FW_PART)
	OleDispatch(doc, "CloseAll")
	Play "%(f)k"
	Play "^(o)"
	CurrentWindow = Wfndwnd("Open Workspace")
	WEditSetText("#1152", project)
	WButtonClick("#1")

	projCol = OleGetProperty(VStudio, "Projects")
	numProjects = OleGetProperty(projCol, "Count")
	For i = 1 To numProjects
		proj = OleDispatch(projCol, "Item", i)
		projName = OleGetProperty(proj, "Name")
		projType = OleGetProperty(proj, "Type")
		If projType = "Build" Then
			configCol = OleGetProperty(proj, "Configurations")
			numConfigs = OleGetProperty(configCol, "Count")
			SetRemotePath(ProjName)
			For j = 1 to numConfigs
				CurrentWindow = Wfndwnd("Visual C++", FW_PART)
				Play "%(b)"
				sleep 1
				Play "o"
				CurrentWindow = Wfndwnd("Set Active Project Configuration", FW_PART,15)
				WListItemClk("#16087", "@"+trim(str(j)))
				WButtonClick("#1")

				config = OleDispatch(configCol, "Item", j)
				
				If rebuildAll then
					CurrentWindow = Wfndwnd("Visual C++", FW_PART)
					' OleDispatch(VStudio, "RebuildAll", config)
'					WMenuSelect("&Build\&Rebuild All")
					Play "%(b)"
					sleep 1
					Play "r"
'					WMenuSelect("&Rebuild All")
				Else
					' OleDispatch(VStudio, "Build", config)
					Play "%(b)"
					sleep 1
					Play "b"
'					WMenuSelect("&Build\&Build")
				End If
				Sleep 30

				numErrors = OleGetProperty(VStudio, "Errors")
				If numErrors > 0 Then
					LOG #10, trim$(str(numErrors))+" Build Errors on: "+OleGetProperty(config, "Name")
					pass = false
				End if
				numErrors = OleGetProperty(VStudio, "Warnings")
				if (numErrors > 0) Then
					LOG #10, trim$(str(numErrors))+" Build Warnings on: "+OleGetProperty(config, "Name")
					pass = false
				End If
				OleReleaseObject(config)
			Next
			OleReleaseObject(configCol)
		End If
		OleReleaseObject(proj)
	next i

	'
	' Clean up
	'
	OleDispatch(VStudio, "Quit")
	OleReleaseObject(ProjCol)
	OleReleaseObject(doc)
	OleReleaseObject(VStudio)
	BuildProject = pass
end sub

sub SetRemotePath(projName$)
	Dim index%, count%, i%
	Play "%{f7}"
	sleep 2
	CurrentWindow = WFndWnd("Project Settings", , 30)
	Play "^{Tab}"
	count = WComboCount("#16434")
	for i = 1 to count
		if trim$(lcase(ComboItemText("#16434", (i)))) <> "all configurations" then
			WComboItemClk("#16434", "@"+trim$(str$(i)))
			WEditSetFocus("#16125")
			WEditSetText("#16125", "e:\samples\"+trim$(projName)+"\"+trim$(projName)+trim$(str$(i))+".xbe")
		end if
	next i
	WButtonClick("#1")
	sleep 2
end sub


function ValidateFilesOnUninstall as integer
	Dim FileArray$(0,3), RegArray$(0,3)
	Dim item%, pass%
	Dim xdkpath$

	pass = true

	VerifyFile(4, FileArray, RegArray)
	'
	' If upper bound > 0 then there were files returned
	'
	if ubound(FileArray, 1) <> 0 then
		pass = false
		LOG #10, "FAIL: The following "+trim$(str$(ubound(FileArray, 1)))+" files are still installed"
		for item = lbound(FileArray, 1) to ubound(FileArray, 1)-1
			LOG #10, "   "+FileArray(item, 0)
		next item
	end if
	'
	' If failed, the flag error
	'
	ValidateFilesOnUninstall = pass
end function