//Copyright (c) Microsoft Corporation.  All rights reserved.

CSceneNavigator* CSceneNavigator::pinstance = 0;
SceneDescription CSceneNavigator::m_SceneDataMap[] = 
{

	/* MainScene Scene */
	{
		XBG_PATH_ANSI"3buttons.xbg", "3buttons", false, NULL, eNullButtonId
	},

	/* MemoryScene Scene */
	{
		XBG_PATH_ANSI"onetext.xbg", "onetext", false, NULL, eNullButtonId
	},

	/* MusicScene Scene */
	{
		XBG_PATH_ANSI"threetext.xbg", "threetext", false, NULL, eNullButtonId
	},

	/* SettingsScene Scene */
	{
		XBG_PATH_ANSI"twotext.xbg", "twotext", false, NULL, eNullButtonId
	},

	/* Keyboard Scene */
	{
		XBG_PATH_ANSI"keyboard.xbg", "keyboard", false, NULL, eNullButtonId
	},

	/* Keypad Scene */
	{
		XBG_PATH_ANSI"keypad.xbg", "keypad", false, NULL, eNullButtonId
	},

	/* KeypadPeriod Scene */
	{
		XBG_PATH_ANSI"keypad.xbg", "keypad", false, NULL, eNullButtonId
	},

	/* NTS_Settings Scene */
	{
		XBG_PATH_ANSI"NTS_Settings.xbg", "NTS_Settings", false, NULL, eNullButtonId
	},

	/* NTS_Hostname Scene */
	{
		XBG_PATH_ANSI"NTS_Hostname.xbg", "NTS_Hostname", true, NULL, eNullButtonId
	},

	/* NTS_HostnameTS Scene */
	{
		XBG_PATH_ANSI"NTS_Hostname.xbg", "NTS_Hostname", true, NULL, eNullButtonId
	},

	/* NTS_PPPoE Scene */
	{
		XBG_PATH_ANSI"NTS_DNS.xbg", "NTS_PPPoE", true, NULL, eNullButtonId
	},

	/* NTS_PPPoETS Scene */
	{
		XBG_PATH_ANSI"NTS_DNS.xbg", "NTS_PPPoE", true, NULL, eNullButtonId
	},

	/* NTS_IP Scene */
	{
		XBG_PATH_ANSI"NTS_IP.xbg", "NTS_IP", true, NULL, eNullButtonId
	},

	/* NTS_DNS Scene */
	{
		XBG_PATH_ANSI"NTS_DNS.xbg", "NTS_DNS", true, NULL, eNullButtonId
	},

	/* NTS_DNSTS Scene */
	{
		XBG_PATH_ANSI"NTS_DNS.xbg", "NTS_DNS", true, NULL, eNullButtonId
	},

	/* CellWall Scene */
	{
		XBG_PATH_ANSI"cellwall.xbg", "cellwall", false, NULL, eNullButtonId
	},

	/* gen_mess_panel Scene */
	{
		XBG_PATH_ANSI"gen_mess_panel.xbg", "gen_mess_panel", false, NULL, eNullButtonId
	},

	/* gen_mess_panel Scene */
	{
		XBG_PATH_ANSI"gen_mess_panel.xbg", "gen_mess_panel", false, NULL, eNullButtonId
	},

	/* ACT_Online Scene */
	{
		XBG_PATH_ANSI"act_online.xbg", "ACT_Online", true, NULL, eNullButtonId
	},

	/* ACT_Id Scene */
	{
		XBG_PATH_ANSI"act_id.xbg", "ACT_Id", true, NULL, eNullButtonId
	},

	/* NTS_Status Scene */
	{
		XBG_PATH_ANSI"NTS_status.xbg", "NTS_Status", false, NULL, eNullButtonId
	},

	/* NTS_ConfigChoice Scene */
	{
		XBG_PATH_ANSI"gen_large_panel.xbg", "NTS_ConfigChoice", false, NULL, eNullButtonId
	},

	/* NTS_CableHelp Scene */
	{
		XBG_PATH_ANSI"NTS_Help.xbg", "NTS_CableHelp", false, NULL, eNullButtonId
	},

	/* NTS_PPPoEHelp Scene */
	{
		XBG_PATH_ANSI"NTS_Help.xbg", "NTS_PPPoEHelp", false, NULL, eNullButtonId
	},

	/* NTS_IPHelp Scene */
	{
		XBG_PATH_ANSI"NTS_Help.xbg", "NTS_IPHelp", false, NULL, eNullButtonId
	},

	/* NTS_DNSHelp Scene */
	{
		XBG_PATH_ANSI"NTS_Help.xbg", "NTS_DNSHelp", false, NULL, eNullButtonId
	},

	/* NTS_ServicesHelp Scene */
	{
		XBG_PATH_ANSI"NTS_Help.xbg", "NTS_ServicesHelp", false, NULL, eNullButtonId
	},

	/* ACT_Credit Scene */
	{
		XBG_PATH_ANSI"act_credit.xbg", "ACT_Credit", true, NULL, eNullButtonId
	},

	/* ACT_Country Scene */
	{
		XBG_PATH_ANSI"act_country.xbg", "ACT_Country", false, NULL, eNullButtonId
	},

	/* ACT_Billing Scene */
	{
		XBG_PATH_ANSI"act_billing.xbg", "ACT_Billing", true, NULL, eNullButtonId
	},

	/* Gen_Large_Panel Scene */
	{
		XBG_PATH_ANSI"gen_large_panel.xbg", "Gen_Large_Panel", true, NULL, eNullButtonId
	},

	/* GEN_Exp_Tumblers Scene */
	{
		XBG_PATH_ANSI"gen_exp_tumblers.xbg", "GEN_Exp_Tumblers", false, NULL, eNullButtonId
	},

	/* GEN_Dob_Tumblers Scene */
	{
		XBG_PATH_ANSI"gen_dob_tumblers.xbg", "GEN_Dob_Tumblers", false, NULL, eNullButtonId
	},

	/* GEN_Welcome_Panel Scene */
	{
		XBG_PATH_ANSI"gen_large_panel.xbg", "GEN_Welcome_Panel", true, NULL, eNullButtonId
	},

	/* ACT_Billing_Country Scene */
	{
		XBG_PATH_ANSI"act_country.xbg", "ACT_Billing_Country", true, NULL, eNullButtonId
	},

	/* Anim_Wait Scene */
	{
		XBG_PATH_ANSI"anim_wait.xbg", "anim_wait", true, NULL, eNullButtonId
	},

	/* Anim_Connect Scene */
	{
		XBG_PATH_ANSI"Anim_Connecting.xbg", "Anim_Connecting", true, NULL, eNullButtonId
	},
	{ NULL, false, NULL },
};