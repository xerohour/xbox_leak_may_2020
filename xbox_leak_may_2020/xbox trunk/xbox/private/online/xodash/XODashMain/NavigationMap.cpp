//Copyright (c) Microsoft Corporation.  All rights reserved.

SceneChild CSceneNavigator::m_SceneHierarchy[] =
{

	/* MainScene Scene */
	{
		{ eMemorySceneId, eMusicSceneId, eSettingsSceneId, eNullSceneId },
		{ eMemoryButtonId, eMusicButtonId, eSettingsButtonId, eNullButtonId },
		{ NULL, NULL, NULL, NULL }
	},

	/* MemoryScene Scene */
	{
		{ eNullSceneId },
		{ eNullButtonId },
		{ NULL }
	},

	/* MusicScene Scene */
	{
		{ eNullSceneId },
		{ eNullButtonId },
		{ NULL }
	},

	/* SettingsScene Scene */
	{
		{ eNullSceneId },
		{ eNullButtonId },
		{ NULL }
	},

	/* Keyboard Scene */
	{
		{ eNullSceneId, eNullSceneId, eNullSceneId, eNullSceneId, eNullSceneId, eNullSceneId, eNullSceneId, eNullSceneId, eNullSceneId, eNullSceneId, eNullSceneId },
		{ eShiftButtonId, eCapsLockButtonId, eAccentsButtonId, eSymbolsButtonId, eSymbolsButtonId, eSpaceButtonId, eBackspaceButtonId, eLessThanButtonId, eGreaterThanButtonId, eDoneButtonId, eNullButtonId },
		{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
	},

	/* Keypad Scene */
	{
		{ eNullSceneId, eNullSceneId, eNullSceneId, eNullSceneId, eNullSceneId },
		{ eBackspaceButtonId, eLessThanButtonId, eGreaterThanButtonId, eDoneButtonId, eNullButtonId },
		{ NULL, NULL, NULL, NULL, NULL }
	},

	/* KeypadPeriod Scene */
	{
		{ eNullSceneId, eNullSceneId, eNullSceneId, eNullSceneId, eNullSceneId },
		{ eBackspaceButtonId, eLessThanButtonId, eGreaterThanButtonId, eDoneButtonId, eNullButtonId },
		{ NULL, NULL, NULL, NULL, NULL }
	},

	/* NTS_Settings Scene */
	{
		{ eNTS_IPId, eNTS_DNSId, eNTS_HostnameId, eNTS_PPPoEId, eACT_OnlineId, eNTS_StatusId, eNullSceneId },
		{ eNTS_settingsIP, eNTS_settingsDNS, eNTS_settingsHost, eNTS_settingsPPPoE, eNTS_settingsCancel, eNTS_settingsConnect, eNullButtonId },
		{ NULL, NULL, NULL, NULL, NULL, NULL, NULL }
	},

	/* NTS_Hostname Scene */
	{
		{ eKeyboardSceneId, eKeyboardSceneId, eNullSceneId, eNullSceneId },
		{ eNTS_HostnameHostname, eNTS_HostnameDomainName, eNTS_HostnameContinue, eNullButtonId },
		{ NULL, NULL, NULL, NULL }
	},

	/* NTS_HostnameTS Scene */
	{
		{ eKeyboardSceneId, eKeyboardSceneId, eNTS_StatusId, eNullSceneId },
		{ eNTS_HostnameHostname, eNTS_HostnameDomainName, eNTS_HostnameContinue, eNullButtonId },
		{ NULL, NULL, NULL, NULL }
	},

	/* NTS_PPPoE Scene */
	{
		{ eKeyboardSceneId, eKeyboardSceneId, eKeyboardSceneId, eNullSceneId, eNullSceneId },
		{ eNTS_PPPoEUserName, eNTS_PPPoEPassword, eNTS_PPPoEServiceName, eNTS_PPPoEContinue, eNullButtonId },
		{ NULL, NULL, NULL, NULL, NULL }
	},

	/* NTS_PPPoETS Scene */
	{
		{ eKeyboardSceneId, eKeyboardSceneId, eKeyboardSceneId, eNTS_StatusId, eNullSceneId },
		{ eNTS_PPPoEUserName, eNTS_PPPoEPassword, eNTS_PPPoEServiceName, eNTS_PPPoEContinue, eNullButtonId },
		{ NULL, NULL, NULL, NULL, NULL }
	},

	/* NTS_IP Scene */
	{
		{ eNullSceneId, eKeypadPeriodSceneId, eKeypadPeriodSceneId, eKeypadPeriodSceneId, eNullSceneId },
		{ eNTS_IPToggle, eNTS_IPIP, eNTS_IPSubnet, eNTS_IPGate, eNullButtonId },
		{ NULL, NULL, NULL, NULL, NULL }
	},

	/* NTS_DNS Scene */
	{
		{ eNullSceneId, eKeypadPeriodSceneId, eKeypadPeriodSceneId, eNullSceneId, eNullSceneId },
		{ eNTS_DNSToggle, eNTS_DNSPrimary, eNTS_DNSSecondary, eNTS_DNSContinue, eNullButtonId },
		{ NULL, NULL, NULL, NULL, NULL }
	},

	/* NTS_DNSTS Scene */
	{
		{ eNullSceneId, eKeypadPeriodSceneId, eKeypadPeriodSceneId, eNTS_StatusId, eNullSceneId },
		{ eNTS_DNSToggle, eNTS_DNSPrimary, eNTS_DNSSecondary, eNTS_DNSContinue, eNullButtonId },
		{ NULL, NULL, NULL, NULL, NULL }
	},

	/* CellWall Scene */
	{
		{ eNullSceneId },
		{ eNullButtonId },
		{ NULL }
	},

	/* gen_mess_panel Scene */
	{
		{ eNullSceneId, eNullSceneId, eNullSceneId, eNullSceneId, eNullSceneId, eNullSceneId },
		{ eMessage_YES, eMessage_CENTER, eMessage_NO, eMessage_BIGYES, eMessage_BIGNO, eNullButtonId },
		{ NULL, NULL, NULL, NULL, NULL, NULL }
	},

	/* gen_mess_panel Scene */
	{
		{ eNullSceneId, eNullSceneId, eNullSceneId, eNullSceneId, eNullSceneId, eNullSceneId },
		{ eMessage_YES, eMessage_CENTER, eMessage_NO, eMessage_BIGYES, eMessage_BIGNO, eNullButtonId },
		{ NULL, NULL, NULL, NULL, NULL, NULL }
	},

	/* ACT_Online Scene */
	{
		{ eNullSceneId, eGEN_Welcome_PanelId, eNTS_ConfigChoiceId, eNullSceneId, eNullSceneId },
		{ eACT_Online01, eACT_Online02, eACT_Online03, eACT_Online04, eNullButtonId },
		{ NULL, NULL, NULL, NULL, NULL }
	},

	/* ACT_Id Scene */
	{
		{ eKeyboardSceneId, eACT_IdId, eGEN_Dob_TumblersId, eACT_CreditId, eACT_OnlineId, eNullSceneId },
		{ eACT_Idname, eACT_Idtag, eACT_IdDOB, eACT_Idcontinue, eACT_idHiddenBadName, eNullButtonId },
		{ NULL, NULL, NULL, NULL, NULL, NULL }
	},

	/* NTS_Status Scene */
	{
		{ eNTS_CableHelpId, eNTS_IPHelpId, eNTS_DNSHelpId, eNTS_ServicesHelpId, eNTS_SettingsId, eACT_OnlineId, eNTS_PPPoETSId, eNTS_HostnameTSId, eNTS_DNSTSId, eNullSceneId },
		{ eNTS_Status01, eNTS_Status02, eNTS_Status03, eNTS_Status04, eNTS_Status05, eNTS_Status06, eNTS_StatusPPPoeInput, eNTS_StatusHostInput, eNTS_StatusDNSInput, eNullButtonId },
		{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
	},

	/* NTS_ConfigChoice Scene */
	{
		{ eNTS_StatusId, eNTS_SettingsId, eNullSceneId },
		{ eNTS_ConfigChoice01, eNTS_ConfigChoice02, eNullButtonId },
		{ NULL, NULL, NULL }
	},

	/* NTS_CableHelp Scene */
	{
		{ eNullSceneId, eNullSceneId },
		{ eNTS_CableHelp01, eNullButtonId },
		{ NULL, NULL }
	},

	/* NTS_PPPoEHelp Scene */
	{
		{ eNullSceneId, eNullSceneId },
		{ eNTS_PPPoEHelp01, eNullButtonId },
		{ NULL, NULL }
	},

	/* NTS_IPHelp Scene */
	{
		{ eNullSceneId, eNullSceneId },
		{ eNTS_IPHelp01, eNullButtonId },
		{ NULL, NULL }
	},

	/* NTS_DNSHelp Scene */
	{
		{ eNullSceneId, eNullSceneId },
		{ eNTS_DNSHelp01, eNullButtonId },
		{ NULL, NULL }
	},

	/* NTS_ServicesHelp Scene */
	{
		{ eNullSceneId, eNullSceneId },
		{ eNTS_ServicesHelp01, eNullButtonId },
		{ NULL, NULL }
	},

	/* ACT_Credit Scene */
	{
		{ eACT_CountryId, eKeyboardSceneId, eKeypadSceneId, eGEN_Exp_TumblersId, eACT_BillingId, eNullSceneId, eNullSceneId },
		{ eACT_Credit01, eACT_Credit02, eACT_Credit03, eACT_Credit04, eACT_Creditdone, eACT_Creditbilling, eNullButtonId },
		{ NULL, NULL, NULL, NULL, NULL, NULL, NULL }
	},

	/* ACT_Country Scene */
	{
		{ eNullSceneId, eNullSceneId, eNullSceneId, eNullSceneId },
		{ eACT_Country01, eACT_Country02, eACT_Country03, eNullButtonId },
		{ NULL, NULL, NULL, NULL }
	},

	/* ACT_Billing Scene */
	{
		{ eKeyboardSceneId, eKeypadSceneId, eKeyboardSceneId, eKeyboardSceneId, eKeypadSceneId, eGen_Large_PanelId, eNullSceneId, eNullSceneId },
		{ eACT_Billing01, eACT_Billing02, eACT_Billing03, eACT_Billing04, eACT_Billing05, eACT_Billingdone, eACT_Billingcredit, eNullButtonId },
		{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
	},

	/* Gen_Large_Panel Scene */
	{
		{ eACT_OnlineId, eACT_OnlineId, eNullSceneId },
		{ eGen_Large_Panel01, eGen_Large_Panel02, eNullButtonId },
		{ NULL, NULL, NULL }
	},

	/* GEN_Exp_Tumblers Scene */
	{
		{ eNullSceneId, eNullSceneId, eNullSceneId },
		{ eGEN_Exp_TumblersMonth, eGEN_Exp_TumblersYear, eNullButtonId },
		{ NULL, NULL, NULL }
	},

	/* GEN_Dob_Tumblers Scene */
	{
		{ eNullSceneId, eNullSceneId, eNullSceneId, eNullSceneId },
		{ eGEN_Dob_TumblersMonth, eGEN_Dob_TumblersDay, eGEN_Dob_TumblersYear, eNullButtonId },
		{ NULL, NULL, NULL, NULL }
	},

	/* GEN_Welcome_Panel Scene */
	{
		{ eACT_Billing_CountryId, eACT_OnlineId, eNullSceneId },
		{ eGen_Welcome_Panel01, eGen_Welcome_Panel02, eNullButtonId },
		{ NULL, NULL, NULL }
	},

	/* ACT_Billing_Country Scene */
	{
		{ eACT_IdId, eACT_IdId, eACT_IdId, eNullSceneId },
		{ eACT_Billing_Country01, eACT_Billing_Country02, eACT_Billing_Country03, eNullButtonId },
		{ NULL, NULL, NULL, NULL }
	},

	/* Anim_Wait Scene */
	{
		{ eNullSceneId },
		{ eNullButtonId },
		{ NULL }
	},

	/* Anim_Connect Scene */
	{
		{ eNullSceneId },
		{ eNullButtonId },
		{ NULL }
	},
	{
		{ eLastSceneId },
		{ eNullButtonId },
		{ NULL }
	}
};