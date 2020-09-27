/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Module implementing cleanup code for old dash files

Module Name:

    olddash.cpp

--*/

#include "xonp.h"
#include "xonver.h"

#ifdef XONLINE_FEATURE_XBOX

//
// To simpify things, have a list of old dash files
//
// Files first, followed by directories
// Directories must be in post-order, otherwise, they will not 
// be deleted properly
//
const PSTR g_rgszOldFiles[] =
{
	"y:\\default.xip",
	"y:\\dvd.xip",
	"y:\\dvdkeyd.bin",
	"y:\\jkeyboard.xip",
	"y:\\Keyboard.xip",
	"y:\\mainmenu5.xip",
	"y:\\Memory2.xip",
	"y:\\memory_files2.xip",
	"y:\\Message.xip",
	"y:\\music2.xip",
	"y:\\music_copy3.xip",
	"y:\\Music_PlayEdit2.xip",
	"y:\\settings3.xip",
	"y:\\Settings_Clock.xip",
	"y:\\settings_language.xip",
	"y:\\settings_list.xip",
	"y:\\settings_panel.xip",
	"y:\\settings_parental.xip",
	"y:\\settings_timezone.xip",
	"y:\\settings_video.xip",
	"y:\\XBox Book.xtf",
	"y:\\Xbox.xtf",
	"y:\\3dinfo\\cameramainTV.cam",
	"y:\\3dinfo\\cellwall.bmp",
	"y:\\3dinfo\\matobj0.mdl",
	"y:\\3dinfo\\matobj1.mdl",
	"y:\\3dinfo\\matobj10.mdl",
	"y:\\3dinfo\\matobj11.mdl",
	"y:\\3dinfo\\matobj12.mdl",
	"y:\\3dinfo\\matobj2.mdl",
	"y:\\3dinfo\\matobj3.mdl",
	"y:\\3dinfo\\matobj4.mdl",
	"y:\\3dinfo\\matobj5.mdl",
	"y:\\3dinfo\\matobj6.mdl",
	"y:\\3dinfo\\matobj7.mdl",
	"y:\\3dinfo\\matobj8.mdl",
	"y:\\3dinfo\\matobj9.mdl",
	"y:\\Audio\\AmbientAudio\\AMB_05_ENGINEROOM_LR.wav",
	"y:\\Audio\\AmbientAudio\\AMB_06_COMMUNICATION_LR.wav",
	"y:\\Audio\\AmbientAudio\\AMB_12_HYDROTHUNDER_LR.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Pinger1.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Steam1.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Steam2.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Steam3.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Steam4.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Steam5.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Steam6.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Steam7.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Voices1.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Voices10.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Voices11.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Voices12.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Voices13.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Voices2.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Voices3.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Voices4.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Voices5.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Voices6.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Voices7.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Voices8.wav",
	"y:\\Audio\\AmbientAudio\\AMB_EC_Voices9.wav",
	"y:\\Audio\\AmbientAudio\\comm static 1.wav",
	"y:\\Audio\\AmbientAudio\\comm static 2.wav",
	"y:\\Audio\\AmbientAudio\\comm static 3.wav",
	"y:\\Audio\\AmbientAudio\\comm static 4.wav",
	"y:\\Audio\\AmbientAudio\\comm voice 1.wav",
	"y:\\Audio\\AmbientAudio\\comm voice 2.wav",
	"y:\\Audio\\AmbientAudio\\comm voice 3.wav",
	"y:\\Audio\\AmbientAudio\\comm voice 4.wav",
	"y:\\Audio\\AmbientAudio\\comm voice 5.wav",
	"y:\\Audio\\AmbientAudio\\comm voice 6.wav",
	"y:\\Audio\\AmbientAudio\\comm voice 7.wav",
	"y:\\Audio\\AmbientAudio\\comm voice 8.wav",
	"y:\\Audio\\AmbientAudio\\comm voice 9.wav",
	"y:\\Audio\\AmbientAudio\\Control Room Loop ver2.wav",
	"y:\\Audio\\AmbientAudio\\Control Room Loop.wav",
	"y:\\Audio\\MainAudio\\Global A Button Select.wav",
	"y:\\Audio\\MainAudio\\Global B Button Back.wav",
	"y:\\Audio\\MainAudio\\Global Completion Beep.wav",
	"y:\\Audio\\MainAudio\\Global Delete_Destroy.wav",
	"y:\\Audio\\MainAudio\\Global Error Message B.wav",
	"y:\\Audio\\MainAudio\\Global Keyboard Stroke 1.wav",
	"y:\\Audio\\MainAudio\\Global Keyboard Stroke 2.wav",
	"y:\\Audio\\MainAudio\\Global Main MenuBack3ver2.wav",
	"y:\\Audio\\MainAudio\\Global Main MenuFwd3ver2.wav",
	"y:\\Audio\\MainAudio\\Global Progress Bar.wav",
	"y:\\Audio\\MainAudio\\Global Scroll Beep.wav",
	"y:\\Audio\\MemoryAudio\\Memory Controller Select.wav",
	"y:\\Audio\\MemoryAudio\\Memory Games Select.wav",
	"y:\\Audio\\MemoryAudio\\Memory Memory Slot Select.wav",
	"y:\\Audio\\MusicAudio\\Games Info Screen In MSurr.wav",
	"y:\\Audio\\MusicAudio\\Games Info Screen In.wav",
	"y:\\Audio\\MusicAudio\\Games Info Screen Out MSurr.wav",
	"y:\\Audio\\MusicAudio\\Games Info Screen Out.wav",
	"y:\\Audio\\MusicAudio\\Music CD Select.wav",
	"y:\\Audio\\SettingsAudio\\Settings Lang SubMenu Sel.wav",
	"y:\\Audio\\SettingsAudio\\Settings Parent SubMenu Sel.wav",
	"y:\\Audio\\TransitionAudio\\Games Main Menu In_LR.wav",
	"y:\\Audio\\TransitionAudio\\Games Main Menu Out_LR.wav",
	"y:\\Audio\\TransitionAudio\\Games Sub Menu In_LR.wav",
	"y:\\Audio\\TransitionAudio\\Games Sub Menu Out_LR.wav",
	"y:\\Audio\\TransitionAudio\\Music Main Menu In_LR.wav",
	"y:\\Audio\\TransitionAudio\\Music Main Menu Out_LR.wav",
	"y:\\Audio\\TransitionAudio\\Music Select Track In_LR.wav",
	"y:\\Audio\\TransitionAudio\\Music Select Track Out_LR.wav",
	"y:\\Audio\\TransitionAudio\\Settings Main Menu In_LR.wav",
	"y:\\Audio\\TransitionAudio\\Settings Main Menu Out_LR.wav",
	"y:\\Audio\\TransitionAudio\\Settings Sub Menu In_LR.wav",
	"y:\\Audio\\TransitionAudio\\Settings Sub Menu Out_LR.wav",
	"y:\\data\\saveimage.xbx",
	"y:\\data\\titleimage.xbx",
	"y:\\images\\logo.bmp",
	"y:\\images\\logow.bmp",
	"y:\\media\\tahoma.ttf",
	"y:\\menus\\avreg.mnu",
	"y:\\menus\\cert.mnu",
	"y:\\menus\\dispset.mnu",
	"y:\\menus\\dns.mnu",
	"y:\\menus\\dumpcache.mnu",
	"y:\\menus\\eeprom.mnu",
	"y:\\menus\\errormsg.mnu",
	"y:\\menus\\gamereg.mnu",
	"y:\\menus\\gateway.mnu",
	"y:\\menus\\ip.mnu",
	"y:\\menus\\language.mnu",
	"y:\\menus\\machine.mnu",
	"y:\\menus\\madisplay.mnu",
	"y:\\menus\\memopts.mnu",
	"y:\\menus\\memory.mnu",
	"y:\\menus\\muformat.mnu",
	"y:\\menus\\muname.mnu",
	"y:\\menus\\muunformat.mnu",
	"y:\\menus\\network.mnu",
	"y:\\menus\\options.mnu",
	"y:\\menus\\root.mnu",
	"y:\\menus\\settings.mnu",
	"y:\\menus\\subnet.mnu",
	"y:\\menus\\sysinfo.mnu",
	"y:\\menus\\timezone.mnu",
	"y:\\menus\\videores.mnu",
	"y:\\sounds\\back_button.wav",
	"y:\\sounds\\keyboard_stroke.wav",
	"y:\\sounds\\menu_item_select.wav",
	"y:\\sounds\\select_button.wav",
	"y:\\Audio\\AmbientAudio",
	"y:\\Audio\\MainAudio",
	"y:\\Audio\\MemoryAudio",
	"y:\\Audio\\MusicAudio",
	"y:\\Audio\\SettingsAudio",
	"y:\\Audio\\TransitionAudio",
	"y:\\Audio",
	"y:\\3dinfo",
	"y:\\data",
	"y:\\images",
	"y:\\media",
	"y:\\menus",
	"y:\\sounds",
//	"y:\\xboxdash.xbe",
//	"y:\\XShell.xbe"
};

const DWORD g_cszOldFiles = sizeof(g_rgszOldFiles) / sizeof(PSTR);

//
// Define one global instance
//

#define READ_ONLY_ATTRIBUTES \
						(FILE_ATTRIBUTE_HIDDEN |\
						 FILE_ATTRIBUTE_SYSTEM |\
						 FILE_ATTRIBUTE_READONLY)
//
// Task do work function
//
HRESULT CXo::xododContinue(
			XONLINETASK_HANDLE	hTask
			)
{
	PXONLINETASK_DEL_OLD_DASH 	pxodod = (PXONLINETASK_DEL_OLD_DASH)hTask;
	WIN32_FILE_ATTRIBUTE_DATA 	fad;
	DWORD						iCurrent;

	Assert(pxodod);

	iCurrent = (pxodod->iCurrent)++;
	if (iCurrent >= g_cszOldFiles)
		return(XONLINETASK_S_SUCCESS);

	if (!GetFileAttributesEx(
				g_rgszOldFiles[iCurrent], GetFileExInfoStandard, &fad))
		goto Exit;

	// Turn off any hidden, system, or read-only bits
	if (fad.dwFileAttributes & READ_ONLY_ATTRIBUTES)
	{
		fad.dwFileAttributes &= ~(READ_ONLY_ATTRIBUTES);
		SetFileAttributes(g_rgszOldFiles[iCurrent], fad.dwFileAttributes);
	}

	if (fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		RemoveDirectory(g_rgszOldFiles[iCurrent]);
	else
		DeleteFile(g_rgszOldFiles[iCurrent]);

Exit:
	return(S_OK);
}

//
// Task close function
//
VOID CXo::xododClose(
			XONLINETASK_HANDLE	hTask
			)
{
	SysFree(hTask);
}

//
// Function to kick off an offline dash claenup.
//
// Clearly this task is not thread-safe so there
// must be only on eo these tasks running
//
// Note: the dash must already have been mounted as
// the y: drive before calling this function
//
HRESULT CXo::XOnlineUpdateDeleteOfflineDash(
			HANDLE						hWorkEvent,
			PXONLINETASK_DEL_OLD_DASH	pxodod
			)
{
	// Set up the autoupdate context
	TaskInitializeContext(&pxodod->xontask);
	pxodod->xontask.pfnContinue = xododContinue;
	pxodod->xontask.pfnClose = xododClose;
	pxodod->xontask.hEventWorkAvailable = hWorkEvent;
	pxodod->iCurrent = 0;

	return(S_OK);
}

#endif
