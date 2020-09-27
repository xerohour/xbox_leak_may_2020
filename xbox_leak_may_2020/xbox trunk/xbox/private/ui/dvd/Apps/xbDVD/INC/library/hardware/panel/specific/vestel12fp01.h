// FILE:			library\hardware\panel\specific\vestel12fp01.h
// AUTHOR:		H.Horak, S. Herr
// COPYRIGHT:	(c) 1999 VIONA Development GmbH & Co. KG.  All Rights Reserved.
// CREATED:		04.08.1999
//
// PURPOSE:		Vestel Front Panel 12FP01
//
// HISTORY:

#ifndef VESTEL12FP01_H
#define VESTEL12FP01_H

#include "futabavfd01.h"
#include "..\generic\panels.h"
#include "library\lowlevel\hardwrio.h"
#include "library\common\krnlsync.h"

class Vestel12FP01Panel : public PollingPanel, public VDMutex 
	{
	private:
		ByteIndexedInOutPort * port;
		BYTE display[DISPLAY_SIZE];
		BYTE preDisplay[DISPLAY_SIZE];
		PanelNumType testNumType;
		PanelDiskType diskType;
		PanelAudioType audioType;
		PanelState panelState, prePanelState;
		PanelTrayStatus trayStatus, preTrayStatus;
		PanelTimeMode timeMode;

		int panelNumericInfo, panelNumericEdit, panelNumericUsed;
		int editPos;
		int displayAngleTime, playerSpeed;
		int preBrightness, brightness;
		int time, angle, title, chapter, pbcEntry, pbcEntries, numOfAngles, numOfTitles, numOfChapters;
		int refCountRepeat;
		int region;
		BOOL enPBC, enPie, power, testMode;
		BYTE	ledAdr;

		void SetPower();
		void ReadKey(BYTE * data);
		void ClrDisplayArray();
		void SetDisplayArray();	// For debugging and testing
		void UpdateDisplay();
		void StepSegment();		// For debugging
		void SetBrightness();

		void SetbitsNumerical(int digit, int value, BYTE *array);
		void ClrTimeDisplay(BYTE *array);
		void ClrTitleDisplay(BYTE *array);
		void ClrChapterDisplay(BYTE *array);
		void ClrNumDisplay(BYTE *array);

		BOOL LockNumDisplay(PanelNumericInfo lock);
		void FreeNumDisplay(PanelNumericInfo free);

		void SetTestResult(BOOL result);

		void SetDiskType();
		void SetAudioType();
		void SetPanelState();
		void SetTrayStatus();

		void SetTime();
		void SetTitle();
		void SetChapter();
		void SetPBCEntry();
		void SetPBCEntries();

		void EditTime();
		void EditTitle();
		void EditChapter();
		void EditPBCEntry();
		void EditAngle();

		void AngleTimer();
		void SetAngle();

		void SetAudioChannels(int channel);

		void WriteSegment(int seg, BOOL state);
		void WriteDigit(int number, int pos);
		void WriteTestNumber (int number);
		void WriteTitle(int number, BOOL enable);
		void WriteChannel(int number, BOOL enable);
		void WriteChapter(int number, BOOL enable);
		void WriteTrack(int number, BOOL enable);
		void WriteNumOfEntries(int number, BOOL enable);
		
		void UpdateActiveSymbols();
		void DeletePie(BYTE *array);
		void EmptyPie(BYTE *array);
		void FullPie(BYTE *array);
		void CrossedPie(BYTE *array);
		void RotatePie(int speed, int roteate, BYTE *array);
		void FlashPie(int speed, BYTE *array);
		void FlashPlaySymbol(int speed, BYTE *array);


	protected:
		void ExecuteOperation(POP pop);
		BOOL PollCEMessage(DWORD & ceMsg);

	public:
		Vestel12FP01Panel(int taskPriority, ByteIndexedInOutPort * port, BOOL powerLEDOn = FALSE);

		POPList GetInitPOPList(int __far & size);

		VirtualUnit * CreateVirtual(void);

		void SetSystemPower(BOOL on);
	};

class VirtualVestel12FP01Panel : public VirtualPanel
	{
	public:
		VirtualVestel12FP01Panel(Vestel12FP01Panel * panel) : VirtualPanel(panel) {;}
	};

#endif
