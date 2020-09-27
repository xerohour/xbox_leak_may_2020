////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//
//  Navigation Module
//
//  Declares DVDNavigationCommand, DVDNavigationControl and
//  DVDNavigationProcessor
//
//////////////////////////////////////////////////////////////////////

#ifndef DVDNAVPU_H
#define DVDNAVPU_H

// Turn debugging of commands on/off
//
// After you turned debugging on by setting NAVPROCDEBUG to 1 make sure to set the debug options correctly.
// Namely these are NPD_LOGTOFILE, NPD_LOGFILENAME, NPD_LOGTODBGOUT which define where the debug output goes and
// NPD_LOGCMDLISTS and NPD_LOGEXECUTION which define what to log.

#define NAVPROCDEBUG 0


#include "Library/Common/Prelude.h"
#include "NavErrors.h"
#include "EventSender.h"
#include "DVDTime.h"

#if NAVPROCDEBUG

#include <stdio.h>

// Log to file yes/no (set file name, too !)
#define NPD_LOGTOFILE	1
#define NPD_LOGFILENAME	"e:\\npdlog.txt"

// Log to debug output yes/no
#define NPD_LOGTODBGOUT	1

#if !NPD_LOGTOFILE && !NPD_LOGTODBGOUT
#error Check debug defines! Set NAVPROCDEBUG to 0 to disable logging
#endif

// Log whole command lists before they are executed (InterpretCommandList())
#define NPD_LOGCMDLISTS 1

// Log each command before it is executed and log the results
#define NPD_LOGEXECUTION 1

// Useful execution logging macros
#if NPD_LOGEXECUTION
#define LOGCOMMAND NPD_LogCommand(com, pc, true);
#define LOGPRM(num) NPD_LogResultPRM(num)
#define LOGEXECUTION(call) { char * buffer = call; NPD_LogResultLn("Exec %s", buffer); delete buffer; }
#else
#define LOGCOMMAND
#define LOGPRM
#define LOGEXECUTION(call)
#endif // NPD_LOGEXECUTION

#else
#define LOGCOMMAND
#define LOGPRM
#define LOGEXECUTION(call)
#endif // NAVPROCDEBUG

//////////////////////////////////////////////////////////////////////
//
//  DVDNavigationCommand Class
//
//////////////////////////////////////////////////////////////////////

class DVDNavigationCommand
	{
	public:
		DWORD dw[2];

		//
		//  Extract segment from 64 bit word
		//
		DWORD SegDW(int pos, int len)
			{
			if (pos >= 32)
				return XTBF(pos - 32, len, FLIPENDIAN(dw[0]));
			else
				return XTBF(pos, len, FLIPENDIAN(dw[1]));
			}

		WORD SegW(int pos, int len)
			{
			if (pos >= 32)
				return (WORD)(XTBF(pos - 32, len, FLIPENDIAN(dw[0])));
			else
				return (WORD)(XTBF(pos, len, FLIPENDIAN(dw[1])));
			}

		//
		//  Check if a certain bit of 64 bit word is set
		//
		BOOL SegBIT(int pos)
			{
			if (pos >= 32)
				return XTBF(pos - 32, FLIPENDIAN(dw[0]));
			else
				return XTBF(pos, FLIPENDIAN(dw[1]));
			}
	};

//////////////////////////////////////////////////////////////////////
//
//  DVDNavigationControl Class
//
//  Base class for high level navigation functions
//
//////////////////////////////////////////////////////////////////////

class DVDNavigationControl
	{
	public:
		// PMH_COMPATIBLE allows setting of PML only when the current level is "undefined"
		// With PMH_ALLOW/FORBID_CHANGE the execution of the SetTmpPML command can be
		// allowed/forbidden

		enum PMLHandling { PMH_COMPATIBLE, PMH_ALLOW_CHANGE, PMH_FORBID_CHANGE, PMH_DEFER_CHANGE };

	protected:
		WORD			sprm[32];
		PMLHandling	pmlHandling;

		//
		//  Freezing stuff
		//

		class DVDNCFreezeState
			{
			public:
				WORD			sprm[32];
				PMLHandling	pmlHandling;
			};

		Error Freeze (DVDNCFreezeState & state);
		Error Defrost(DVDNCFreezeState & state);

	public:
		virtual Error GoProgram(WORD pgn) = 0;
		virtual Error GoCell(WORD cell) = 0;

		virtual Error GoPrevProgram(void) = 0;
		virtual Error GoNextProgram(void) = 0;
		virtual Error GoTopProgram(void) = 0;
		virtual Error GoPrevCell(void) = 0;
		virtual Error GoNextCell(void) = 0;
		virtual Error GoTopCell(void) = 0;
		virtual Error GoTopProgramChain(void) = 0;
		virtual Error GoTailProgramChain(void) = 0;

		virtual Error GoProgramChain(WORD pgcn) = 0;
		virtual Error GoPartOfTitle(WORD pttn) = 0;

		virtual Error GoPrevProgramChain(void) = 0;
		virtual Error GoNextProgramChain(void) = 0;
		virtual Error GoUpProgramChain(void) = 0;

   	virtual Error Exit(Error err = GNR_OK) = 0;
   	virtual Error GoTitle(WORD title, DWORD flags = DDPSPF_NONE) = 0;
   	virtual Error GoVideoTitleSetTitle(WORD title, BOOL toPause = FALSE) = 0;
   	virtual Error GoVideoTitleSetPartOfTitle(WORD title, WORD ptt) = 0;
   	virtual Error GoSystemSpaceProgramChain(WORD pgcn, WORD domainID, WORD menuID) = 0;
   	virtual Error GoSystemSpaceTitleSet(WORD titleSet, WORD title, WORD domainID, WORD menuID) = 0;
   	virtual Error CallSystemSpaceProgramChain(WORD pgcn, WORD domainID, WORD menuID, WORD resumeCell) = 0;
		virtual Error GoTrack(WORD track, WORD titleGroupNumber) = 0;

		virtual Error ResumeFromSystemSpace(void) = 0;

		virtual Error GetSPRM(int index, WORD & val) { val = sprm[index]; GNRAISE_OK;}
		virtual Error SetSPRM(int index, WORD val) {sprm[index] = val; GNRAISE_OK;}

		// Only returns PMH_ALLOW/FORBID/DEFER_CHANGE
		virtual PMLHandling PMLChangeAllowed(void) = 0;
	};

//////////////////////////////////////////////////////////////////////
//
//  DVDNavigationProcessor Class
//
//  Processes low level navigation commands
//
//////////////////////////////////////////////////////////////////////

class DVDNavigationProcessor
	{
	public:
		// This type specifies if processing of commands is to be terminated
		// or just defered because of the SetTMPPML command
		enum NavCommandDone { NCD_NO, NCD_YES, NCD_SETTMPPML_DEFERED };

	private:
		DWORD seed;
		WORD	gprm[16];
		BOOL	gprmMode[16];
		DWORD	gprmTimerStart[16];

		int	recursionLevel;
		WORD	requestedPML;

#if NAVPROCDEBUG
		FILE*	logFile;
		bool	npd_newInstruction;
#endif

	protected:
		DVDNavigationControl	*	navigationControl;

		virtual WORD GetGPRM(int index);
		virtual void SetGPRM(int index, WORD val);
		virtual void SetGPRMMode(int index, WORD val, BOOL counter);

		Error SetHL_BTTN(WORD button);

		BOOL Compare(WORD val1, WORD val2);
		BOOL CompareCP(DVDNavigationCommand com, int cp1, int cp2);

		Error ExecOp(int op, WORD & dst, WORD & src);
		Error ExecOpSP(DVDNavigationCommand com, int sp1, int sp2);

		WORD GetPRM(WORD num);

		Error ExecNop(DVDNavigationCommand com, int & pc, NavCommandDone & done);
		Error ExecGoTo(DVDNavigationCommand com, int & pc, NavCommandDone & done, WORD & pml);
		Error ExecCompareGoTo(DVDNavigationCommand com, int & pc, NavCommandDone & done);
		Error ExecLink(DVDNavigationCommand com, int & pc, NavCommandDone & done);
		Error ExecCompareLink(DVDNavigationCommand com, int & pc, NavCommandDone & done);
		Error ExecJump(DVDNavigationCommand com, int & pc, NavCommandDone & done);
		Error ExecCompareJump(DVDNavigationCommand com, int & pc, NavCommandDone & done);
		Error ExecSetSystem(DVDNavigationCommand com, int & pc, NavCommandDone & done);
		Error ExecCompareSetSystem(DVDNavigationCommand com, int & pc, NavCommandDone & done);
		Error ExecSetSystemLink(DVDNavigationCommand com, int & pc, NavCommandDone & done);
		Error ExecSet(DVDNavigationCommand com, int & pc, NavCommandDone & done);
		Error ExecCompareSet(DVDNavigationCommand com, int & pc, NavCommandDone & done);
		Error ExecSetLink(DVDNavigationCommand com, int & pc, NavCommandDone & done);
		Error ExecLinkSIns(DVDNavigationCommand com, int & pc, NavCommandDone & done);
		Error ExecSetCompareLinkSIns(DVDNavigationCommand com, int & pc, NavCommandDone & done);
		Error ExecCompareAndSetLinkSIns(DVDNavigationCommand com, int & pc, NavCommandDone & done);
		Error ExecCompareSetAndLinkSIns(DVDNavigationCommand com, int & pc, NavCommandDone & done);

		//
		// Debug functions
		//

#if NAVPROCDEBUG

		void NPD_LogString(const char * string);
		//void NPD_Log(int pc, const char * format, ...);
		//void NPD_Log(const char * format, ...);
		void NPD_LogLn(int pc, const char * format, ...);
		void NPD_LogLn(const char * format, ...);
		void NPD_LogResultLn(const char * format, ...);

		// Make string stuff...
		char * NPD_MakeString(const char * format, ...);
		char * NPD_MakeGoToString(DVDNavigationCommand com);
		char * NPD_MakePRMString(WORD num, bool runtime = false);
		void NPD_MakeCompareArgs(DVDNavigationCommand com, int cp1, int cp2, char* & arg1, char* & arg2, bool runtime = false);
		char * NPD_MakeCompareString(DVDNavigationCommand com, int cp1, int cp2, bool runtime = false);
		char * NPD_MakeLinkSInsString(DVDNavigationCommand com);
		char * NPD_MakeLinkString(DVDNavigationCommand com);
		char * NPD_MakeJumpString(DVDNavigationCommand com);
		char * NPD_MakeSetSystemString(DVDNavigationCommand com, bool runtime = false);
		char * NPD_MakeOpString(int opcode);
		char * NPD_MakeSetString(DVDNavigationCommand com, int sp1, int sp2, bool runtime = false);


//		void NPD_LogGoTo(DVDNavigationCommand com);
		void NPD_LogDualCommand(int pc, char * pPart1, char * pPart2);
		void NPD_LogTripleCommand(int pc, char * pPart1, char * pPart2, char * pPart3, int ampersandPos);

		// Goto group
		void NPD_LogNop(DVDNavigationCommand com, int pc);
		void NPD_LogGoTo(DVDNavigationCommand com, int pc);
		void NPD_LogCompareGoTo(DVDNavigationCommand com, int pc, bool runtime = false);

		// Link group
		void NPD_LogLink(DVDNavigationCommand com, int pc);
		void NPD_LogCompareLink(DVDNavigationCommand com, int pc, bool runtime = false);

		// Jump group
		void NPD_LogJump(DVDNavigationCommand com, int pc);
		void NPD_LogCompareJump(DVDNavigationCommand com, int pc, bool runtime = false);

		// SetSystem group
		void NPD_LogSetSystem(DVDNavigationCommand com, int pc, bool runtime = false);
		void NPD_LogCompareSetSystem(DVDNavigationCommand com, int pc, bool runtime = false);
		void NPD_LogSetSystemLink(DVDNavigationCommand com, int pc, bool runtime = false);

		// Set group
		void NPD_LogSet(DVDNavigationCommand com, int pc, bool runtime = false);
		void NPD_LogCompareSet(DVDNavigationCommand com, int pc, bool runtime = false);
		void NPD_LogSetLink(DVDNavigationCommand com, int pc, bool runtime = false);

		void NPD_LogSetCompareLinkSIns(DVDNavigationCommand com, int pc, bool runtime = false);
		void NPD_LogCompareAndSetLinkSIns(DVDNavigationCommand com, int pc, bool runtime = false);
		void NPD_LogCompareSetAndLinkSIns(DVDNavigationCommand com, int pc, bool runtime = false);

		void NPD_LogResultPRM(WORD num);

		void NPD_LogCommand(DVDNavigationCommand com, int pc, bool runtime = false);
		void NPD_LogCommandList(DVDNavigationCommand * com, int pc, int num);

		WORD NPD_GetSPRM(WORD num);

		void NPD_LogSPRM(void);
		void NPD_LogGPRM(void);
		void NPD_LogPRM(void);
#endif

		int compareField;
		int branchField;

	public:
		DVDNavigationProcessor(void);

#if NAVPROCDEBUG
		~DVDNavigationProcessor(void);
#endif


		Error ClearGPRMs(void);

		Error InterpretCommand(DVDNavigationCommand com, int & pc, NavCommandDone & done, WORD & pml);
		Error InterpretCommandList(DVDNavigationCommand * com, int & pc, int num, NavCommandDone & done, WORD & pml);

		BOOL IsBasicCellLinkCommand(DVDNavigationCommand com);
		Error GetBasicCellLinkTarget(DVDNavigationCommand com, WORD & cellID);

		void SetNavigationControl(DVDNavigationControl * control) {navigationControl = control;}

		//
		// Freezing stuff
		//

		class DVDNPFreezeState
			{
			public:
				DWORD seed;
				WORD	gprm[16];
				WORD	gprmMode;
				DWORD	gprmTimerStart[16];
			};

		Error Freeze (DVDNPFreezeState & state);
		Error Defrost(DVDNPFreezeState & state);
	};

#endif
