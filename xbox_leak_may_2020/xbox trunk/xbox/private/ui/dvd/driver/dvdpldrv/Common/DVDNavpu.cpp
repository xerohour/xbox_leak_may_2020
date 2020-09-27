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
//  Navigation Command Processing Unit
//
//////////////////////////////////////////////////////////////////////

#include "DVDNavpu.h"
#include "Library/lowlevel/timer.h"
#include "Library/Common/vddebug.h"

#define DDP_LOG_DVD_COMMANDS 0

#if DDP_LOG_DVD_COMMANDS
#define LDC DP
#else
#define LDC //
#endif

// Include header files needed for debug output
#if NAVPROCDEBUG
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#endif

//////////////////////////////////////////////////////////////////////
//
//  DVDNavigationControl Class
//
//////////////////////////////////////////////////////////////////////

//
//  Save current state
//
//  GNR_OK
//

Error DVDNavigationControl::Freeze(DVDNCFreezeState & state)
	{
	int i;

	for (i=0; i<32; i++)
		state.sprm[i] = sprm[i];

	GNRAISE_OK;
	}

//
//  Return to previously saved state
//
//  GNR_OK
//

Error DVDNavigationControl::Defrost(DVDNCFreezeState & state)
	{
	int i;

	//
	//  Check if parental level is high enough
	//

	if (sprm[13] < state.sprm[13])
		GNRAISE(GNR_PARENTAL_LEVEL_TOO_LOW);

	//
	//  Now set the SPRMs with exception of the following:
	//	  0: Menu Description Language Code
	//  12: Parental Country Code
	//  13: Parental Level
	//  14: Player Configuration For Video
	//  16: Initial Language Code For Audio Streams
	//  17: Initial Language Code Extension For Audio Streams
	//  18: Initial Language Code For Subpicture Streams
	//  19: Initial Language Code Extension For Subpicture Streams
	//  This is done because those are user settings.
	//

	for (i=1; i<12; i++)
		sprm[i] = state.sprm[i];

	sprm[15] = state.sprm[15];

	for (i=20; i<32; i++)
		sprm[i] = state.sprm[i];

	GNRAISE_OK;
	}

//////////////////////////////////////////////////////////////////////
//
//  Navigation Processor Class
//
//////////////////////////////////////////////////////////////////////

//
//  Constructor
//

DVDNavigationProcessor::DVDNavigationProcessor(void)
	{
	recursionLevel = 0;

#if NAVPROCDEBUG
	logFile = NULL;
#endif
	}

//
//  GPRM Access
//

WORD DVDNavigationProcessor::GetGPRM(int index)
	{
	LDC("GetGPRM %d is %04x", index, gprm[index]);

	if (gprmMode[index])
	   return gprm[index] + (WORD)((Timer.GetMilliTicks() - gprmTimerStart[index]) / 1000);
	else
		return gprm[index];
	}

void DVDNavigationProcessor::SetGPRM(int index, WORD val)
	{
	LDC("SetGRPM %d to %04x", index, val);

	gprm[index] = val;
	if (gprmMode[index])
		gprmTimerStart[index] = Timer.GetMilliTicks();
	}

void DVDNavigationProcessor::SetGPRMMode(int index, WORD val, BOOL counter)
	{
	gprm[index] = val;
	if (counter)
		gprmTimerStart[index] = Timer.GetMilliTicks();

	gprmMode[index] = counter;
	}

//
//  Clear GPRMs
//
//  GNR_OK
//

Error DVDNavigationProcessor::ClearGPRMs(void)
	{
	int i;

	for(i=0; i<16; i++)
		{
		SetGPRM(i, 0);
		gprmMode[i] = FALSE;
		}

	GNRAISE_OK;
	}

//
//  Saturation arithmetic help functions
//

WORD Saturate(long val)
	{
	if (val < 0) return 0;
	else if (val > 0xffff) return 0xffff;
	else return (WORD)val;
	}

WORD Saturate(DWORD val)
	{
	if (val > 0xffff) return 0xffff;
	else return (WORD)val;
	}

//
//  Execute operation
//
//  GNR_OK
//  GNR_ILLEGAL_NAVIGATION_COMMAND
//

Error DVDNavigationProcessor::ExecOp(int op, WORD & dst, WORD & src)
	{
	WORD tmp;

	switch (op)
		{
		case 1: dst = src; GNRAISE_OK;
		case 2: tmp = dst; dst = src; src = tmp; GNRAISE_OK;
		case 3: dst = Saturate((long)dst + (long)src); GNRAISE_OK;
		case 4: dst = Saturate((long)dst - (long)src); GNRAISE_OK;
		case 5: dst = Saturate((DWORD)dst * (DWORD)src); GNRAISE_OK;
		case 6: dst = src ? dst / src : 0xffff; GNRAISE_OK;
		case 7: dst = src ? dst % src : 0xffff; GNRAISE_OK;
		case 8:
			if (src)
				{
				seed = 1664525L * seed + 1013904223L;
				dst = (WORD)(seed % src) + 1;
				}
			else
				dst = 0;

			GNRAISE_OK;
			break;
		case 9: dst &= src; GNRAISE_OK;
		case 10: dst |= src; GNRAISE_OK;
		case 11: dst ^= src; GNRAISE_OK;
		}

	GNRAISE(GNR_ILLEGAL_NAVIGATION_COMMAND);
	}

//
//  Execute operation SP
//
//  GNR_OK
//  GNR_ILLEGAL_NAVIGATION_COMMAND
//

Error DVDNavigationProcessor::ExecOpSP(DVDNavigationCommand com, int sp1, int sp2)
	{
	int s1;
	WORD imm, w1, w2, sprm;

	imm = com.SegW(sp2, 16);
	s1 = com.SegDW(sp1, 4);

	if (com.SegBIT(60))
		{
		w1 = GetGPRM(s1);
		GNREASSERT(ExecOp(com.SegDW(56, 4), w1, imm));
		SetGPRM(s1, w1);
		}
	else if (XTBF(7, imm))
		{
		GNREASSERT(navigationControl->GetSPRM(XTBF(0, 5, imm), sprm));
		w1 = GetGPRM(s1);
		GNREASSERT(ExecOp(com.SegDW(56, 4), w1, sprm));
		SetGPRM(s1, w1);
		}
	else
		{
		imm = (WORD)XTBF(0, 4, imm);
		w1 = GetGPRM(s1);
		w2 = GetGPRM(imm);
		GNREASSERT(ExecOp(com.SegDW(56, 4), w1, w2));
		SetGPRM(imm, w2);
		SetGPRM(s1, w1);
		}

	// Log result
	LOGPRM((WORD)XTBF(0, 4, s1));

	GNRAISE_OK;
	}

//
//  Compare two values
//

BOOL DVDNavigationProcessor::Compare(WORD val1, WORD val2)
	{
	switch (compareField)
		{
		case 1: return (val1 & val2) != 0;	// Bitwise compare ???
		case 2: return val1 == val2;			// Equal
		case 3: return val1 != val2;			// Not Equal
		case 4: return val1 >= val2;			// Greater than or equal to
		case 5: return val1 > val2;			// Greater than
		case 6: return val1 <= val2;			// Less than or equal to
		case 7: return val1 < val2;			// Less than
		}

	return FALSE;
	}

//
//  Get register value
//

WORD DVDNavigationProcessor::GetPRM(WORD num)
	{
	WORD sprm;

	if (XTBF(7, num))
		{
		navigationControl->GetSPRM(XTBF(0, 5, num), sprm);
		return sprm; // ???????????
		}
	else
		return GetGPRM(XTBF(0, 4, num));
	}

//
//  Compare
//

BOOL DVDNavigationProcessor::CompareCP(DVDNavigationCommand com, int cp1, int cp2)
	{
	WORD c1, c2, r2;

	c1 = GetPRM(com.SegW(cp1, 8));

	if (com.SegBIT(55)) // immediate
		{
		c2 = com.SegW(cp2, 16);
		}
	else
		{
		r2 = com.SegW(cp2, 8);
		c2 = GetPRM(r2);
		if (r2 == 0x88 && ((c1 & 0xfc00) == 0) && ((c1 & 0x3f) != 0))
			{
//			DP("Shinji RA Munita workaround");

			c1 <<= 10;
			}
		}

//	DP("Compare %08x with %08x", c1, c2);
#if NPD_LOGEXECUTION
	BOOL result = Compare(c1, c2);
	NPD_LogResultLn("Compare result: %s", result ? "true" : "false");
	return result;
#else
	return Compare(c1, c2);
#endif
	}

//
//  Execution methods for the different commands
//
//  GNR_OK
//

Error DVDNavigationProcessor::ExecNop(DVDNavigationCommand com, int & pc, NavCommandDone & done)
	{
	LOGCOMMAND;

	pc++;
	done = NCD_NO;

	GNRAISE_OK;
	}

//
//  Execute Go To
//
//  GNR_OK
//  GNR_UNEXPECTED_NAVIGATION_ERROR
//  GNR_ILLEGAL_NAVIGATION_COMMAND
//

Error DVDNavigationProcessor::ExecGoTo(DVDNavigationCommand com, int & pc, NavCommandDone & done, WORD & pml)
	{
	WORD operand = com.SegW(0, 16);
	WORD sprm;

	LOGCOMMAND

	// Log execution
	LOGEXECUTION(NPD_MakeGoToString(com));

	done = NCD_NO;

	switch (branchField)
		{
		case 1: // GoTo
			pc = XTBF(0, 8, operand);
			break;
		case 2: // Break
			done = NCD_YES;
			break;
		case 3: // SetTmpPML
			switch (navigationControl->PMLChangeAllowed())
				{
				case DVDNavigationControl::PMH_ALLOW_CHANGE:
					navigationControl->SetSPRM(13, (WORD)XTBF(8, 4, operand));
					pc = XTBF(0, 8, operand);
					break;
				case DVDNavigationControl::PMH_FORBID_CHANGE:
					pc++;
					break;
				case DVDNavigationControl::PMH_DEFER_CHANGE:
					GNREASSERT(navigationControl->GetSPRM(13, sprm));
					if (sprm < (WORD)XTBF(8, 4, operand))
						{
						// Current PML is too low -> ask for permission
						done = NCD_SETTMPPML_DEFERED;
						pml = (WORD)XTBF(8, 4, operand);
						}
					else
						{
						// Current PML is higher -> set it and continue
						navigationControl->SetSPRM(13, (WORD)XTBF(8, 4, operand));
						pc = XTBF(0, 8, operand);
						}
					break;
				default:
					done = NCD_YES;
					GNRAISE(GNR_UNEXPECTED_NAVIGATION_ERROR);
				}
			break;
		default:
			done = NCD_YES;
			GNRAISE(GNR_ILLEGAL_NAVIGATION_COMMAND);
		}

	GNRAISE_OK;
	}

//
//  Execute Compare Go To
//

Error DVDNavigationProcessor::ExecCompareGoTo(DVDNavigationCommand com, int & pc, NavCommandDone & done)
	{
	WORD pml; // dummy here

	LOGCOMMAND;

   if (CompareCP(com, 32, 16))
	 	return ExecGoTo(com, pc, done, pml);
	else
		return ExecNop(com, pc, done);
	}

//
//  Set Highlight Button
//
//  Misc. Errors
//

Error DVDNavigationProcessor::SetHL_BTTN(WORD button)
	{
	if (button)
		return navigationControl->SetSPRM(8, button);
	else
		GNRAISE_OK;
	}

//
//  Execute Link
//

Error DVDNavigationProcessor::ExecLink(DVDNavigationCommand com, int & pc, NavCommandDone & done)
	{
	LOGCOMMAND;

	// Log execution
#if NPD_LOGEXECUTION
	if (branchField != 1)
	LOGEXECUTION(NPD_MakeLinkString(com));
#endif

	switch (branchField)
		{
		case 1: // LinkSIns
			return ExecLinkSIns(com, pc, done);
			break;
		case 2: // LinkTKN
			break;
		case 3: // LinkDLISTN
			break;
		case 4: // LinkPGCN
			GNREASSERT(navigationControl->GoProgramChain(com.SegW(0, 15)));
			break;
		case 5: // LinkPTTN
			GNREASSERT(SetHL_BTTN(com.SegW(10, 6)));
			GNREASSERT(navigationControl->GoPartOfTitle(com.SegW(0, 10)));
			break;
		case 6: // LinkPGN
			GNREASSERT(SetHL_BTTN(com.SegW(10, 6)));
			GNREASSERT(navigationControl->GoProgram(com.SegW(0, 7)));
			break;
		case 7: // LinkCN
			GNREASSERT(SetHL_BTTN(com.SegW(10, 6)));
			GNREASSERT(navigationControl->GoCell(com.SegW(0, 8)));
			break;
		case 10: // LinkTKNGP
			break;
		case 11: // LinkDLISTNGP
			break;
		case 12: // LinkPGCNGP
			break;
		case 14: // LinkPGNGP
			break;
		case 15: // LinkCNGP
			break;
		default:
			GNRAISE(GNR_ILLEGAL_NAVIGATION_COMMAND);
		}

	done = NCD_YES;

	GNRAISE_OK;
	}

//
//  Execute Link By Sub Instruction
//

Error DVDNavigationProcessor::ExecLinkSIns(DVDNavigationCommand com, int & pc, NavCommandDone & done)
	{
	GNREASSERT(SetHL_BTTN(com.SegW(10, 6)));

	// Log execution
	LOGEXECUTION(NPD_MakeLinkSInsString(com));

	switch (com.SegDW(0, 8))
		{
		case 0:
			done = NCD_NO;
			pc++;
			GNRAISE_OK;
			break;
		case 1: GNREASSERT(navigationControl->GoTopCell()); break;
		case 2: GNREASSERT(navigationControl->GoNextCell()); break;
		case 3: GNREASSERT(navigationControl->GoPrevCell()); break;
		case 5: GNREASSERT(navigationControl->GoTopProgram()); break;
		case 6: GNREASSERT(navigationControl->GoNextProgram()); break;
		case 7: GNREASSERT(navigationControl->GoPrevProgram()); break;
		case 9: GNREASSERT(navigationControl->GoTopProgramChain()); break;
		case 10: GNREASSERT(navigationControl->GoNextProgramChain()); break;
		case 11: GNREASSERT(navigationControl->GoPrevProgramChain()); break;
		case 12: GNREASSERT(navigationControl->GoUpProgramChain()); break;
		case 13: GNREASSERT(navigationControl->GoTailProgramChain()); break;
		case 16: GNREASSERT(navigationControl->ResumeFromSystemSpace()); break;
		default:
			GNRAISE(GNR_ILLEGAL_NAVIGATION_COMMAND);
		}

	done = NCD_YES;

	GNRAISE_OK;
	}

//
//  Execute Compare Link
//

Error DVDNavigationProcessor::ExecCompareLink(DVDNavigationCommand com, int & pc, NavCommandDone & done)
	{
	LOGCOMMAND;

	if (CompareCP(com, 32, 16))
		return ExecLink(com, pc, done);

	pc++;
	done = NCD_NO;

	GNRAISE_OK;
	}

//
//  Execute Jump
//

Error DVDNavigationProcessor::ExecJump(DVDNavigationCommand com, int & pc, NavCommandDone & done)
	{
	WORD domain;

	// Log the command itself (only when not already logged)
	LOGCOMMAND;

	// Log execution
	LOGEXECUTION(NPD_MakeJumpString(com));

   switch (branchField)
   	{
   	case 1: // Exit
   		GNREASSERT(navigationControl->Exit());
   		break;
   	case 2: // JumpTT
   		GNREASSERT(navigationControl->GoTitle(com.SegW(16, 7)));
   		break;
   	case 3: // JumpVTS_TT
   		GNREASSERT(navigationControl->GoVideoTitleSetTitle(com.SegW(16, 7)));
   		break;
   	case 5: // JumpVTS_PTT
   		GNREASSERT(navigationControl->GoVideoTitleSetPartOfTitle(com.SegW(16, 7), com.SegW(32, 10)));
   		break;
   	case 6: // JumpSS
   		domain = com.SegW(22, 2);
   		if (domain != 2)
   			GNREASSERT(navigationControl->GoSystemSpaceProgramChain(com.SegW(32, 15), domain, com.SegW(16, 4)));
   		else
   			GNREASSERT(navigationControl->GoSystemSpaceTitleSet(com.SegW(24, 7), com.SegW(32, 7), domain, com.SegW(16, 4)));
   		break;
   	case 8: // CallSS
  			GNREASSERT(navigationControl->CallSystemSpaceProgramChain(com.SegW(32, 15), com.SegW(22, 2), com.SegW(16, 4), com.SegW(24, 8)));
   		break;
		case 9: // JumpTT_GR
			break;
		case 10: // JumpTK - Start the presentation of a Track in the specified TT_GR (DVD-Audio only)
			GNREASSERT(navigationControl->GoTrack(com.SegW(16, 7), com.SegW(32, 4)));
			break;
		case 11: // AMGM_RSM
			break;
   	}

	done = NCD_YES;

	GNRAISE_OK;
	}

//
//  Execute Compare Jump
//

Error DVDNavigationProcessor::ExecCompareJump(DVDNavigationCommand com, int & pc, NavCommandDone & done)
	{
	LOGCOMMAND;

	if (CompareCP(com, 8, 0))
		return ExecJump(com, pc, done);

	pc++;
	done = NCD_NO;

	GNRAISE_OK;
	}

//
//  Execute Set System
//

Error DVDNavigationProcessor::ExecSetSystem(DVDNavigationCommand com, int & pc, NavCommandDone & done)
	{
	BOOL imm = com.SegBIT(60);
	WORD w1, w2, w3;

	LOGCOMMAND;

	// Log execution
	LOGEXECUTION(NPD_MakeSetSystemString(com));

	switch (com.SegDW(56, 4))
		{
		case 1: // SetSTN
		   w1 = (WORD)com.SegDW(32, 4);
		   w2 = (WORD)com.SegDW(24, 7);
		   w3 = (WORD)com.SegDW(16, 4);
		   if (!imm)
		   	{
		   	w1 = GetGPRM(w1);
		   	w2 = GetGPRM(w2);
		   	w3 = GetGPRM(w3);
		   	}

		   if (com.SegBIT(39)) GNREASSERT(navigationControl->SetSPRM(1, w1));
		   if (com.SegBIT(31)) GNREASSERT(navigationControl->SetSPRM(2, w2));
		   if (com.SegBIT(23)) GNREASSERT(navigationControl->SetSPRM(3, w3));

			break;
		case 2: // SetNVTMR
			w2 = (WORD)com.SegDW(16, 15);
			if (imm)
				w1 = (WORD)com.SegDW(32, 16);
			else
				w1 = GetGPRM(com.SegDW(32, 4));

			GNREASSERT(navigationControl->SetSPRM(10, w2));
			GNREASSERT(navigationControl->SetSPRM(9, w1));
			break;
		case 3: // SetGPRMMD
			if (imm)
				w1 = (WORD)com.SegDW(32, 16);
			else
				w1 = GetGPRM(com.SegDW(32, 4));

			w2 = (WORD)com.SegDW(16, 4);

			SetGPRMMode(w2, w1, com.SegBIT(23));
			break;
		case 4: // SetAMXMD
			if (imm)
				w1 = (WORD)com.SegDW(32, 16);
			else
				w1 = GetGPRM(com.SegDW(32, 4));

			GNREASSERT(navigationControl->SetSPRM(11, w1));
			break;
		case 6: // SetHL_BTNN
			if (imm)
				w1 = (WORD)com.SegDW(26, 6);
			else
				w1 = (WORD)XTBF(10, 6, GetGPRM(com.SegDW(16, 4)));

			GNREASSERT(SetHL_BTTN(w1));	// Logging is done inside this function
			break;
		default:
			GNRAISE(GNR_ILLEGAL_NAVIGATION_COMMAND);
		}

	pc++;
	done = NCD_NO;

	GNRAISE_OK;
	}

//
//  Execute Compare Set System
//

Error DVDNavigationProcessor::ExecCompareSetSystem(DVDNavigationCommand com, int & pc, NavCommandDone & done)
	{
	LOGCOMMAND;
	if (CompareCP(com, 8, 0))
		return ExecSetSystem(com, pc, done);

	pc++;
	done = NCD_NO;

	GNRAISE_OK;
	}

//
//  Execute Set System Link
//

Error DVDNavigationProcessor::ExecSetSystemLink(DVDNavigationCommand com, int & pc, NavCommandDone & done)
	{
	LOGCOMMAND;
	GNREASSERT(ExecSetSystem(com, pc, done));
	pc--;
	return ExecLink(com, pc, done);
	}

//
//  Execute Set
//

Error DVDNavigationProcessor::ExecSet(DVDNavigationCommand com, int & pc, NavCommandDone & done)
	{
	LOGCOMMAND;
	GNREASSERT(ExecOpSP(com, 32, 16));

	pc++;
	done = NCD_NO;

	GNRAISE_OK;
	}

//
//  Execute Compare Set
//

Error DVDNavigationProcessor::ExecCompareSet(DVDNavigationCommand com, int & pc, NavCommandDone & done)
	{
	LOGCOMMAND;
	if (CompareCP(com, 40, 0))
		{
		GNREASSERT(ExecOpSP(com, 32, 16));
		}

	pc++;
	done = NCD_NO;

	GNRAISE_OK;
	}

//
//  Execute Set Link
//

Error DVDNavigationProcessor::ExecSetLink(DVDNavigationCommand com, int & pc, NavCommandDone & done)
	{
	LOGCOMMAND;
	GNREASSERT(ExecOpSP(com, 32, 16));
	return ExecLink(com, pc, done);
	}

//
//  Execute Set Compare Link By Sub Instruction
//

Error DVDNavigationProcessor::ExecSetCompareLinkSIns(DVDNavigationCommand com, int & pc, NavCommandDone & done)
	{
	LOGCOMMAND;
	GNREASSERT(ExecOpSP(com, 48, 32));
	if (CompareCP(com, 48, 16))
		return ExecLinkSIns(com, pc, done);

	pc++;
	done = NCD_NO;

	GNRAISE_OK;
	}

//
//  Execute Compare And Set Link By Sub Instruction
//

Error DVDNavigationProcessor::ExecCompareAndSetLinkSIns(DVDNavigationCommand com, int & pc, NavCommandDone & done)
	{
	LDC("CompareAndSetLinkSIns");
	if (!com.SegBIT(60))
		{
		if (CompareCP(com, 32, 16))
		 	{
			GNREASSERT(ExecOpSP(com, 48, 40));
			return ExecLinkSIns(com, pc, done);
			}
		}
	else
		{
		if (CompareCP(com, 24, 16))
		 	{
			GNREASSERT(ExecOpSP(com, 48, 32));
			return ExecLinkSIns(com, pc, done);
			}
		}

	pc++;
	done = NCD_NO;

	GNRAISE_OK;
	}

//
//  Execute Compare Set And Link By Sub Instruction
//

Error DVDNavigationProcessor::ExecCompareSetAndLinkSIns(DVDNavigationCommand com, int & pc, NavCommandDone & done)
	{
	LDC("CompareSetAndLinkSIns");
	if (!com.SegBIT(60))
		{
		if (CompareCP(com, 32, 16))
		 	{
			GNREASSERT(ExecOpSP(com, 48, 40));
			}
		}
	else
		{
		if (CompareCP(com, 24, 16))
		 	{
			GNREASSERT(ExecOpSP(com, 48, 32));
			}
		}

	return ExecLinkSIns(com, pc, done);
	}

BOOL DVDNavigationProcessor::IsBasicCellLinkCommand(DVDNavigationCommand com)
	{
	DWORD opcode = com.SegDW(48, 16);
	DWORD oplink = com.SegDW(0, 16);

	if (opcode == 0x2007 ||
		 opcode == 0x2001 && (oplink & 0x00ff) >= 0x0001 && (oplink & 0x00ff) <= 0x0003)
		{
		return (oplink & 0xfc00) == 0x0000;
		}
	else if (opcode == 0x0000)
		return TRUE;
	else
		return FALSE;
	}

Error DVDNavigationProcessor::GetBasicCellLinkTarget(DVDNavigationCommand com, WORD & cellID)
	{
	DWORD opcode = com.SegDW(48, 16);
	DWORD oplink = com.SegDW(0, 16);

	if (opcode == 0x2007)
		{
		cellID = (WORD)(oplink & 0x00ff);
		}
	else if (opcode == 0x0000)
		{
		cellID++; // NOP
		}
	else if (opcode == 0x2001)
		{
		switch (oplink & 0x00ff)
			{
			case 0x0001:         ; break;
			case 0x0002: cellID++; break;
			case 0x0003: cellID--; break;
			default:
				GNRAISE(GNR_ILLEGAL_NAVIGATION_COMMAND);
			}
		}
	else
		GNRAISE(GNR_ILLEGAL_NAVIGATION_COMMAND);

	GNRAISE_OK;
	}

//
//  Interpret Command
//
//  GNR_OK
//  GNR_ILLEGAL_NAVIGATION_COMMAND
//  GNR_UNEXPECTED_NAVIGATION_ERROR
//

Error DVDNavigationProcessor::InterpretCommand(DVDNavigationCommand com, int & pc, NavCommandDone & done, WORD & pml)
	{
	int cmdid1;

//	DP("PC : %02x Command %08lx%08lx", pc, FLIPENDIAN(com.dw[0]), FLIPENDIAN(com.dw[1]));

#if NPD_LOGEXECUTION
	// The new instruction is used to avoid logging combined instructions more than once
	// if the second and/or the third instruction are executed. This flag is reset
	// by the NPD_LogCommand()
	npd_newInstruction = true;
#endif

	cmdid1 = com.SegDW(61, 3);

	compareField = com.SegDW(52, 3);
	branchField = com.SegDW(48, 4);

	switch (cmdid1)
		{
		case 0:
			if (!branchField)
				return ExecNop(com, pc, done);
			else if (!compareField)
				return ExecGoTo(com, pc, done, pml);
			else
				return ExecCompareGoTo(com, pc, done);
			break;
		case 1:
			if (!com.SegBIT(60))
				{
				if (!compareField)
					return ExecLink(com, pc, done);
				else
					return ExecCompareLink(com, pc, done);
				break;
				}
			else
				{
				if (!compareField)
					return ExecJump(com, pc, done);
				else
					return ExecCompareJump(com, pc, done);
				break;
				}
		case 2:
			if (compareField)
				return ExecCompareSetSystem(com, pc, done);
			else if (branchField)
				return ExecSetSystemLink(com, pc, done);
			else
				return ExecSetSystem(com, pc, done);
			break;
		case 3:
			if (compareField)
				return ExecCompareSet(com, pc, done);
			else if (branchField)
				return ExecSetLink(com, pc, done);
			else
				return ExecSet(com, pc, done);
			break;
		case 4:
			return ExecSetCompareLinkSIns(com, pc, done);
			break;
		case 5:
			return ExecCompareAndSetLinkSIns(com, pc, done);
			break;
		case 6:
			return ExecCompareSetAndLinkSIns(com, pc, done);
			break;
		default:
			done = NCD_NO;
			GNRAISE(GNR_ILLEGAL_NAVIGATION_COMMAND);
			break;
		}

	GNRAISE_OK;
	}

//
//  Interpret Command List
//
//  GNR_OK
//  GNR_UNEXPECTED_NAVIGATION_ERROR
//

Error DVDNavigationProcessor::InterpretCommandList(DVDNavigationCommand * com, int & pc, int num, NavCommandDone & done, WORD & pml)
	{
#if NPD_LOGCMDLISTS
	NPD_LogCommandList(com, pc, num);
#endif

#if NPD_LOGEXECUTION
	NPD_LogLn("*** Begin executing command list, recursionLevel = %i", recursionLevel + 1);
	NPD_LogPRM();
#endif

	if (recursionLevel < 200)
		{
		recursionLevel++;

		do {
			GNREASSERT(InterpretCommand(com[pc-1], pc, done, pml));
			} while (done == NCD_NO && pc <= num);

		recursionLevel--;
		}
	else
   	navigationControl->Exit(GNR_UNEXPECTED_NAVIGATION_ERROR);

#if NPD_LOGEXECUTION
	NPD_LogString("*** End executing command list\n");
#endif

	GNRAISE_OK;
	}

//
//  Freeze current state
//
//  GNR_OK
//

Error DVDNavigationProcessor::Freeze(DVDNPFreezeState & state)
	{
	int i;

	//
	// Save data
	//

	state.seed = seed;
	state.gprmMode = 0;

	for (i=0; i<16; i++)
		{
		state.gprm[i] = gprm[i];
		state.gprmMode |= MKBFW(i, gprmMode[i]);

		if (gprmMode[i])
			{
			// Create relative time
			state.gprmTimerStart[i] = Timer.GetMilliTicks() - gprmTimerStart[i];
			}
		}

	GNRAISE_OK;
	}

//
//  Restore previously saved state
//
//  GNR_OK
//

Error DVDNavigationProcessor::Defrost(DVDNPFreezeState & state)
	{
	int i;

	//
	// Restore data
	//

	seed = state.seed;
	for (i=0; i<16; i++)
		{
		gprm[i] = state.gprm[i];
		gprmMode[i] = (BOOL) XTBFW(i, state.gprmMode);

		if (gprmMode[i])
			{
			// Recreate absolute time
			gprmTimerStart[i] = state.gprmTimerStart[i] + Timer.GetMilliTicks();
			}
		}

	GNRAISE_OK;
	}

#if NAVPROCDEBUG

// Print a log message either to debug out or to a file
void DVDNavigationProcessor::NPD_LogString(const char * string)
	{
	// Log to file
#if NPD_LOGTOFILE
	if (!logFile)
		{
		// Open logfile if neccessary
		logFile = fopen(NPD_LOGFILENAME, "a+t");
		if (logFile)
			{
			char buffer[20], buffer1[20];
			fprintf(logFile, "\n\n*** Logging started at %s on %s\n", _strtime(buffer), _strdate(buffer1));
			}
		}
	if (logFile)
		{
		fprintf(logFile, string);
		fflush(logFile);
		}
#endif

	// Log to debug out
#if NPD_LOGTODBGOUT
	OutputDebugString(string);
#endif
	}

// These function below help logging things by using variable arguments and printf() - kompatible syntax
// to log strings

void DVDNavigationProcessor::NPD_LogLn(int pc, const char * format, ...)
	{
	va_list	list;
	char		buffer1[200], buffer2[210];

	va_start(list, format);

	// Create the string to log
	_vsnprintf(buffer1, 200, format, list);
	_snprintf(buffer2, 210, "%04i: %s\n", pc, buffer1);

	NPD_LogString(buffer2);
	}

void DVDNavigationProcessor::NPD_LogResultLn(const char * format, ...)
	{
	va_list	list;
	char		buffer[200];

	va_start(list, format);

	// Create the string to log
	_vsnprintf(buffer, 200, format, list);

	NPD_LogLn("      %s", buffer);
	}

void DVDNavigationProcessor::NPD_LogLn(const char * format, ...)
	{
	va_list	list;
	char		buffer1[200], buffer2[210];

	va_start(list, format);

	// Create the string to log
	_vsnprintf(buffer1, 200, format, list);
	_snprintf(buffer2, 210, "%s\n", buffer1);

	NPD_LogString(buffer2);
	}

//
// Each of the NPD_MakeXXXX functions returns a pointer to a string that describes that particular
// command. You are responsible for the deletion of the string memory


char * DVDNavigationProcessor::NPD_MakePRMString(WORD num, bool runtime)
	{
	char *string = new char[50];

	if (runtime)
		{
		if (XTBF(7, num))
			_snprintf(string, 50, "SPRM(%i)[0x%x]", XTBF(0, 5, num), GetPRM(num));
		else
			_snprintf(string, 50, "GPRM(%i)[0x%x]", XTBF(0, 4, num), GetPRM(num));
		}
	else
		{
		if (XTBF(7, num))
			_snprintf(string, 50, "SPRM(%i)", XTBF(0, 5, num));
		else
			_snprintf(string, 50, "GPRM(%i)", XTBF(0, 4, num));
		}

	return string;
	}


// Retrieve the arguments for a compare operation
void DVDNavigationProcessor::NPD_MakeCompareArgs(DVDNavigationCommand com, int cp1, int cp2, char* &arg1, char* &arg2, bool runtime)
	{

	arg1 = NPD_MakePRMString(com.SegW(cp1, 8), runtime);

	//arg1 = GetPRM(com.SegW(cp1, 8));

	if (com.SegBIT(55)) // immediate
		{
		arg2 = new char[20];
		_snprintf(arg2, 20, "0x%x(Imm)", com.SegW(cp2, 16));
		}
	else
		arg2 = NPD_MakePRMString(com.SegW(cp2, 8), runtime);
	}

// build a string that describes the comparison
char * DVDNavigationProcessor::NPD_MakeCompareString(DVDNavigationCommand com, int cp1, int cp2, bool runtime)
	{
	char * buffer = new char[100];
	char opString[5];
	char *arg1, *arg2;

	NPD_MakeCompareArgs(com, cp1, cp2, arg1, arg2, runtime);

	if (compareField == 1)
		{
		_snprintf(buffer, 100, "Compare (%s & &s) != 0", arg1, arg2);
		}
	else
		{
		if (compareField < 8)
			{
			switch (compareField)
				{
				case 2: strcpy(opString, "=="); break;
				case 3: strcpy(opString, "!="); break;
				case 4: strcpy(opString, ">="); break;
				case 5: strcpy(opString, ">"); break;
				case 6: strcpy(opString, "<="); break;
				case 7: strcpy(opString, "<"); break;
				}
			_snprintf(buffer, 100, "Compare %s %s %s", arg1, opString, arg2);
			}
		else
			{
			_snprintf(buffer, 100, "Illegal compare Field (%i) Op1:%s, Op2:%s", compareField, arg1, arg2);
			}
		}

	delete arg1;
	delete arg2;

	return buffer;
	}

char * DVDNavigationProcessor::NPD_MakeString(const char * format, ...)
	{
	va_list	list;
	char*		buffer = new char[200];

	va_start(list, format);

	// Create the string to log
	_vsnprintf(buffer, 200, format, list);

	return buffer;
	}


char * DVDNavigationProcessor::NPD_MakeGoToString(DVDNavigationCommand com)
	{
	int operand = com.SegDW(0, 16);

	switch(branchField)
		{
		case 1: return NPD_MakeString("GoTo %04i", operand); break;
		case 2: return NPD_MakeString("Break"); break;
		case 3: return NPD_MakeString("SetTmpPML %i", XTBF(0, 8, operand)); break;
		default:
			return NPD_MakeString("GoTo: Illegal branch field (%i)", branchField); break;
		}
	}

char * DVDNavigationProcessor::NPD_MakeLinkSInsString(DVDNavigationCommand com)
	{
	return NPD_MakeString("LinkSIns %d", com.SegDW(0, 8));
	}


char * DVDNavigationProcessor::NPD_MakeLinkString(DVDNavigationCommand com)
	{
	switch (branchField)
		{
		case 1: // LinkSIns
			return NPD_MakeLinkSInsString(com);
			break;
		case 2: // LinkTKN
			return NPD_MakeString("LinkTKN command");
			break;
		case 3: // LinkDLISTN
			return NPD_MakeString("LinkDLISTN command");
			break;
		case 4: // LinkPGCN
			return NPD_MakeString("LinkPGCN %d", com.SegW(0, 15));
			break;
		case 5: // LinkPTTN
			return NPD_MakeString("LinkPTTN %d", com.SegW(0, 10));
			break;
		case 6: // LinkPGN
			return NPD_MakeString("LinkPGN %d", com.SegW(0, 7));
			break;
		case 7: // LinkCN
			return NPD_MakeString("LinkCN %d", com.SegW(0, 8));
			break;
		case 10: // LinkTKNGP
			return NPD_MakeString("LinkTKNGP command");
			break;
		case 11: // LinkDLISTNGP
			return NPD_MakeString("LinkDLISTNGP command");
			break;
		case 12: // LinkPGCNGP
			return NPD_MakeString("LinkPGCNP command");
			break;
		case 14: // LinkPGNGP
			return NPD_MakeString("LinkPGNGP command");
			break;
		case 15: // LinkCNGP
			return NPD_MakeString("LinkCNGP command");
			break;
		default:
			return NPD_MakeString("Illegal LINK command");
		}

	}

char * DVDNavigationProcessor::NPD_MakeJumpString(DVDNavigationCommand com)
	{
	WORD domain;

   switch (branchField)
   	{
   	case 1: // Exit
			return NPD_MakeString("Exit");
   		break;
   	case 2: // JumpTT
			return NPD_MakeString("JumpTT %d", com.SegW(16, 7));
   		break;
   	case 3: // JumpVTS_TT
			return NPD_MakeString("JumpVTS_TT %d", com.SegW(16, 7));
   		break;
   	case 5: // JumpVTS_PTT
			return NPD_MakeString("JumpVTS_PTT %d %d", com.SegW(16, 7), com.SegW(32, 10));
   		break;
   	case 6: // JumpSS
   		domain = com.SegW(22, 2);
			if (domain != 2)
				return NPD_MakeString("JumpSS %d VMGM_PCGN:%i MenuId:%i", com.SegW(22, 2), com.SegW(32, 15), com.SegW(16, 4));
			else
				return NPD_MakeString("JumpSS %d VTS_TTN:%i VTSN:%i MenuId:%i", com.SegW(22, 2), com.SegW(32, 7), com.SegW(24, 7), com.SegW(16, 4));
   		break;
   	case 8: // CallSS
			return NPD_MakeString("CallSS");
   		break;
		case 9: // JumpTT_GR
			return NPD_MakeString("JumpTT_GR");
			break;
		case 10: // JumpTK - Start the presentation of a Track in the specified TT_GR (DVD-Audio only)
			return NPD_MakeString("JumpTK %i %i", com.SegW(16, 7), com.SegW(32, 4));
			break;
		case 11: // AMGM_RSM
			return NPD_MakeString("AMGM_RSM");
			break;
		default:
			return NPD_MakeString("Invalid jump (%i)", branchField);
			break;
   	}
	}

char * DVDNavigationProcessor::NPD_MakeSetSystemString(DVDNavigationCommand com, bool runtime)
	{
	BOOL imm = com.SegBIT(60);
	WORD w1, w2, w3;

	switch (com.SegDW(56, 4))
		{
		case 1: // SetSTN
			char *buffer1, *buffer2, *buffer3;
			char *result;
		   w1 = (WORD)com.SegDW(32, 4);
		   w2 = (WORD)com.SegDW(24, 7);
		   w3 = (WORD)com.SegDW(16, 4);
		   if (!imm)
		   	{
				// Non-immediate
				char *prmString1,*prmString2,*prmString3;

		   	w1 = GetGPRM(w1);
		   	w2 = GetGPRM(w2);
		   	w3 = GetGPRM(w3);

				prmString1=NPD_MakePRMString((WORD)XTBF(0,4,w1), runtime);
				prmString2=NPD_MakePRMString((WORD)XTBF(0,4,w2), runtime);
				prmString3=NPD_MakePRMString((WORD)XTBF(0,4,w3), runtime);

				buffer1 = NPD_MakeString("%s->SPRM(1)", prmString1);
				buffer2 = NPD_MakeString("%s->SPRM(2)", prmString2);
				buffer3 = NPD_MakeString("%s->SPRM(3)", prmString3);

				delete prmString1;
				delete prmString2;
				delete prmString3;
		   	}
			else
				{
				// Immediate
				buffer1 = NPD_MakeString("0x%x->SPRM(1)", w1);
				buffer2 = NPD_MakeString("0x%x->SPRM(2)", w2);
				buffer3 = NPD_MakeString("0x%x->SPRM(3)", w3);
				}

			result= NPD_MakeString("SetSTN %s %s %s",
				com.SegBIT(39) ? buffer1 : "",
				com.SegBIT(31) ? buffer2 : "",
				com.SegBIT(23) ? buffer3 : "");

			delete buffer1;
			delete buffer2;
			delete buffer3;

			return result;
			break;
		case 2: // SetNVTMR
			{
			char *string; // Temporary string
			char *result;

			// Extract argument
			w2 = (WORD)com.SegDW(16, 15);

			if (imm)
				{
				string = new char[20];
				_snprintf(string, 20, "0x%x",(WORD)com.SegDW(32, 16));
				}
			else
				string = NPD_MakePRMString((WORD)com.SegDW(32, 4), runtime);

			result = NPD_MakeString("SetNVTMR 0x%x->SPRM(10) %s->SPRM(9)", w2, string);

			delete string;

			return result;
			}
			break;
		case 3: // SetGPRMMD
			{
			char *string; // Temporary string
			char *result;

			if (imm)
				{
				string = new char[20];
				_snprintf(string, 20, "0x%x",(WORD)com.SegDW(32, 16));
				}
			else
				string = NPD_MakePRMString((WORD)com.SegDW(32, 4), runtime);

			w2 = (WORD)com.SegDW(16, 4);

			result = NPD_MakeString("SetGPRMMD %s->GPRM(%i) Mode:%i",string, w2, com.SegBIT(23));

			delete string;

			return result;
			}
			break;
		case 4: // SetAMXMD
			{
			char *string; // Temporary string
			char *result;

			if (imm)
				{
				string = new char[20];
				_snprintf(string, 20, "0x%x",(WORD)com.SegDW(32, 16));
				}
			else
				string = NPD_MakePRMString((WORD)com.SegDW(32, 4), runtime);

			result = NPD_MakeString("SetAMXMD %s->SPRM(11)", string);
			return result;
			}
			break;
		case 6: // SetHL_BTNN
			if (imm)
				w1 = (WORD)com.SegDW(26, 6);
			else
				w1 = (WORD)XTBF(10, 6, GetGPRM(com.SegDW(16, 4)));

			return NPD_MakeString("SetHL_BTNN %i->SPRM(8) %s", w1, imm ? "(Imm)" : "");
			break;
		default:
			return NPD_MakeString("Illegal SetSystem command (%i)", com.SegDW(56, 4));
		}

	}

char * DVDNavigationProcessor::NPD_MakeOpString(int opcode)
	{
	switch (opcode)
		{
		case 1:return NPD_MakeString("Mov"); break;
		case 2:return NPD_MakeString("Swp"); break;
		case 3:return NPD_MakeString("Add"); break;
		case 4:return NPD_MakeString("Sub"); break;
		case 5:return NPD_MakeString("Mul"); break;
		case 6:return NPD_MakeString("Div"); break;
		case 7:return NPD_MakeString("Mod"); break;
		case 8:return NPD_MakeString("Rnd"); break;
		case 9:return NPD_MakeString("And"); break;
		case 10:return NPD_MakeString("Or"); break;
		case 11:return NPD_MakeString("Xor"); break;
		default:return NPD_MakeString("Invalid Set (%i)", opcode); break;
		}
	}

char * DVDNavigationProcessor::NPD_MakeSetString(DVDNavigationCommand com, int sp1, int sp2, bool runtime)
	{
	int s1;
	WORD imm;
	char * opString = NPD_MakeOpString(com.SegDW(56, 4));
	char * result = NULL;

	imm = com.SegW(sp2, 16);
	s1 = com.SegDW(sp1, 4);

	// Immediate?
	if (com.SegBIT(60))
		{
		result =  NPD_MakeString("%sI GPRM(%i), 0x%x", opString, s1, imm);
		}
	else
		{
		char * secondArg = NPD_MakePRMString(imm, runtime);
		result =  NPD_MakeString("%s GPRM(%i), %s", opString, s1, secondArg);
		delete secondArg;
		}

	delete opString;
	return result;
	}

// These two functions help logging multi-part commands

void DVDNavigationProcessor::NPD_LogDualCommand(int pc, char * pPart1, char * pPart2)
	{
	NPD_LogLn(pc, "%s %s", pPart1, pPart2);

	delete pPart1;
	delete pPart2;
	}

void DVDNavigationProcessor::NPD_LogTripleCommand(int pc, char * pPart1, char * pPart2, char * pPart3, int ampersandPos)
	{
	switch(ampersandPos)
		{
		case 1:NPD_LogLn(pc, "%s & %s %s", pPart1, pPart2, pPart3); break;
		case 2:NPD_LogLn(pc, "%s %s & %s", pPart1, pPart2, pPart3); break;
		default:NPD_LogLn(pc, "%s %s %s", pPart1, pPart2, pPart3); break;
		}

	delete pPart1;
	delete pPart2;
	delete pPart3;
	}

//
// Each function below logs a specific command. These functions are called from NPD_LogCommand()
//

void DVDNavigationProcessor::NPD_LogNop(DVDNavigationCommand com, int pc)
	{
	NPD_LogLn(pc, "Nop");
	}

void DVDNavigationProcessor::NPD_LogGoTo(DVDNavigationCommand com, int pc)
	{
	char * pString = NPD_MakeGoToString(com);

	NPD_LogLn(pc,pString);

	delete pString;
	}

void DVDNavigationProcessor::NPD_LogCompareGoTo(DVDNavigationCommand com, int pc, bool runtime)
	{
	NPD_LogDualCommand(pc, NPD_MakeCompareString(com, 32, 16, runtime), NPD_MakeGoToString(com));
	}

void DVDNavigationProcessor::NPD_LogLink(DVDNavigationCommand com, int pc)
	{
	char * pString = NPD_MakeLinkString(com);
	NPD_LogLn(pc, pString);
	delete pString;
	}

void DVDNavigationProcessor::NPD_LogCompareLink(DVDNavigationCommand com, int pc, bool runtime)
	{
	NPD_LogDualCommand(pc, NPD_MakeCompareString(com, 32, 16, runtime), NPD_MakeLinkString(com));
	}

void DVDNavigationProcessor::NPD_LogJump(DVDNavigationCommand com, int pc)
	{
	char * pString = NPD_MakeJumpString(com);
	NPD_LogLn(pc, pString);
	delete pString;
	}

void DVDNavigationProcessor::NPD_LogCompareJump(DVDNavigationCommand com, int pc, bool runtime)
	{
	NPD_LogDualCommand(pc, NPD_MakeCompareString(com, 8, 0, runtime), NPD_MakeJumpString(com));
	}

void DVDNavigationProcessor::NPD_LogSetSystem(DVDNavigationCommand com, int pc, bool runtime)
	{
	char * pString = NPD_MakeSetSystemString(com);
	NPD_LogLn(pc, pString);
	delete pString;
	}

void DVDNavigationProcessor::NPD_LogCompareSetSystem(DVDNavigationCommand com, int pc, bool runtime)
	{
	NPD_LogDualCommand(pc, NPD_MakeCompareString(com, 8, 0, runtime), NPD_MakeSetSystemString(com));
	}

void DVDNavigationProcessor::NPD_LogSetSystemLink(DVDNavigationCommand com, int pc, bool runtime)
	{
	NPD_LogDualCommand(pc, NPD_MakeLinkString(com), NPD_MakeSetSystemString(com));
	}

void DVDNavigationProcessor::NPD_LogSet(DVDNavigationCommand com, int pc, bool runtime)
	{
	char * pString = NPD_MakeSetString(com, 32, 16, runtime);
	NPD_LogLn(pc, pString);
	delete pString;
	}

void DVDNavigationProcessor::NPD_LogCompareSet(DVDNavigationCommand com, int pc, bool runtime)
	{
	NPD_LogDualCommand(pc, NPD_MakeCompareString(com, 40, 0, runtime), NPD_MakeSetString(com, 32, 16, runtime));
	}

void DVDNavigationProcessor::NPD_LogSetLink(DVDNavigationCommand com, int pc, bool runtime)
	{
	NPD_LogDualCommand(pc, NPD_MakeLinkString(com), NPD_MakeSetString(com, 32, 16, runtime));
	}

void DVDNavigationProcessor::NPD_LogSetCompareLinkSIns(DVDNavigationCommand com, int pc, bool runtime)
	{
	NPD_LogTripleCommand(pc, NPD_MakeSetString(com, 48, 32, runtime),
		NPD_MakeCompareString(com, 48, 16, runtime),
		NPD_MakeLinkSInsString(com), 0);

	}

void DVDNavigationProcessor::NPD_LogCompareAndSetLinkSIns(DVDNavigationCommand com, int pc, bool runtime)
	{
	if (!com.SegBIT(60))
		{
		NPD_LogTripleCommand(pc, NPD_MakeCompareString(com, 32, 16, runtime),
			NPD_MakeSetString(com, 48, 40, runtime),
			NPD_MakeLinkSInsString(com), 1);
		}
	else
		{
		NPD_LogTripleCommand(pc, NPD_MakeCompareString(com, 24, 16, runtime),
			NPD_MakeSetString(com, 48, 32, runtime),
			NPD_MakeLinkSInsString(com), 1);
		}
	}

void DVDNavigationProcessor::NPD_LogCompareSetAndLinkSIns(DVDNavigationCommand com, int pc, bool runtime)
	{
	if (!com.SegBIT(60))
		{
		NPD_LogTripleCommand(pc, NPD_MakeCompareString(com, 32, 16, runtime),
			NPD_MakeSetString(com, 48, 40, runtime),
			NPD_MakeLinkSInsString(com), 2);
		}
	else
		{
		NPD_LogTripleCommand(pc, NPD_MakeCompareString(com, 24, 16, runtime),
			NPD_MakeSetString(com, 48, 32, runtime),
			NPD_MakeLinkSInsString(com), 2);
		}
	}

// Log a the value of a single parameter

void DVDNavigationProcessor::NPD_LogResultPRM(WORD num)
	{
	char * string = NPD_MakePRMString(num, true);
	NPD_LogResultLn("result: %s", string);
	delete string;
	}

// Log a single instruction (only when npd_newInstruction is true, otherwise nothing happens.
// This mechanism is to prevent multiple logging of composite instructions):

void DVDNavigationProcessor::NPD_LogCommand(DVDNavigationCommand com, int pc, bool runtime)
	{
	int cmdId1;

//	DP("PC : %02x Command %08lx%08lx", pc, FLIPENDIAN(com.dw[0]), FLIPENDIAN(com.dw[1]));

#if NPD_LOGEXECUTION
	if (npd_newInstruction)
		{
#endif
		cmdId1 = com.SegDW(61, 3);

		compareField = com.SegDW(52, 3);
		branchField = com.SegDW(48, 4);

		switch (cmdId1)
			{
			case 0:
				if (!branchField)
					NPD_LogNop(com, pc);
				else if (!compareField)
					NPD_LogGoTo(com, pc);
				else
					NPD_LogCompareGoTo(com, pc, runtime);
				break;
			case 1:
				if (!com.SegBIT(60))
					{
					if (!compareField)
						NPD_LogLink(com, pc);
					else
						NPD_LogCompareLink(com, pc, runtime);
					break;
					}
				else
					{
					if (!compareField)
						NPD_LogJump(com, pc);
					else
						NPD_LogCompareJump(com, pc, runtime);
					break;
					}
			case 2:
				if (compareField)
					NPD_LogCompareSetSystem(com, pc, runtime);
				else if (branchField)
					NPD_LogSetSystemLink(com, pc, runtime);
				else
					NPD_LogSetSystem(com, pc, runtime);
				break;
			case 3:
				if (compareField)
					NPD_LogCompareSet(com, pc, runtime);
				else if (branchField)
					NPD_LogSetLink(com, pc, runtime);
				else
					NPD_LogSet(com, pc, runtime);
				break;
			case 4:
				NPD_LogSetCompareLinkSIns(com, pc, runtime);
				break;
			case 5:
				NPD_LogCompareAndSetLinkSIns(com, pc, runtime);
				break;
			case 6:
				NPD_LogCompareSetAndLinkSIns(com, pc, runtime);
				break;
			default:
				NPD_LogLn(pc, "Illegal Navigation Command (%i)", cmdId1);
				break;
			}
#if NPD_LOGEXECUTION
		}

	// The new instruction is used to avoid logging combined instructions more than once
	// if the second and/or the third instruction are executed. This flag is set
	// by the InterpretCommand()
	npd_newInstruction = false;
#endif

	}

// Log whole command list to a file or debug output

void DVDNavigationProcessor::NPD_LogCommandList(DVDNavigationCommand * com, int pc, int num)
	{
	NPD_LogString("*** Command List start\n");
	do {
		npd_newInstruction = true;
		NPD_LogCommand(com[pc-1], pc);
		pc++;
		} while (pc <= num);

	NPD_LogString("*** Command List end\n");
	}

// Close the log file on deletion

DVDNavigationProcessor::~DVDNavigationProcessor(void)
	{
	if (logFile != NULL)
		{
		fclose(logFile);
		logFile = NULL;
		}
	}

// Get the value of a SPRM
WORD DVDNavigationProcessor::NPD_GetSPRM(WORD num)
	{
	WORD w;
	navigationControl->GetSPRM(num, w);

	return w;
	}

// Log all SPRM values

void DVDNavigationProcessor::NPD_LogSPRM(void)
	{
	for (int i = 0; i < 24; i+=4)
		{
		NPD_LogResultLn("SPRM(%02i):0x%04x SPRM(%02i):0x%04x SPRM(%02i):0x%04x SPRM(%02i):0x%04x",
			i, NPD_GetSPRM(i), i+1, NPD_GetSPRM(i+1), i+2, NPD_GetSPRM(i+2), i+3, NPD_GetSPRM(i+3));
		}
	}

// Log all GPRM values

void DVDNavigationProcessor::NPD_LogGPRM(void)
	{
	for (int i = 0; i < 16; i+=4)
		{
		NPD_LogResultLn("GPRM(%02i):0x%04x GPRM(%02i):0x%04x GPRM(%02i):0x%04x GPRM(%02i):0x%04x",
			i, GetGPRM(i), i+1, GetGPRM(i+1), i+2, GetGPRM(i+2), i+3, GetGPRM(i+3));
		}
	}

// Log all parameters, GPRM and SPRM

void DVDNavigationProcessor::NPD_LogPRM(void)
	{
	NPD_LogGPRM();
	NPD_LogSPRM();
	}


#endif
