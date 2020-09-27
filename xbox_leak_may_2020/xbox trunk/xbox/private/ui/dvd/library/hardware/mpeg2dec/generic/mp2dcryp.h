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

#ifndef MP2DCRYP_H
#define MP2DCRYP_H

#include "library/common/prelude.h"
#include "library/common/gnerrors.h"

#define GNR_CSS_NOT_SUPPORTED			MKERR(ERROR, DECRYPTION, OPERATION, 0x00)
#define GNR_AUTHENTICATION_FAILED	MKERR(ERROR, DECRYPTION, OPERATION, 0x00)

enum MPEG2DVDAuthenticationCommand
	{
	M2D_CHECK_DISK_KEY,				// 0
	M2D_CHECK_TITLE_KEY,          // 1
	M2D_START_AUTHENTICATION,     // 2
	M2D_READ_CHALLENGE_KEY,       // 3
	M2D_WRITE_BUS_KEY,            // 4
	M2D_WRITE_CHALLENGE_KEY,      // 5
	M2D_READ_BUS_KEY,             // 6
	M2D_WRITE_DISK_KEY,           // 7
	M2D_WRITE_TITLE_KEY,          // 8
	M2D_COMPLETE_AUTHENTICATION,  // 9
	M2D_CANCEL_AUTHENTICATION     // 10
	};

#ifndef ONLY_EXTERNAL_VISIBLE

#include "library/lowlevel/hardwrio.h"

class MPEG2DVDDecryption
	{
	public:
		virtual Error DoAuthenticationCommand(MPEG2DVDAuthenticationCommand com, DWORD sector, BYTE __far * key) = 0;
		virtual Error Initialize(void) {GNRAISE_OK;}
		virtual Error PacketReset(void) {GNRAISE_OK;}
	};

class PioneerDecryption : public MPEG2DVDDecryption
	{
	protected:
		BitOutputPort	*	sck;
		BitInputPort	*	mo;
		BitOutputPort	*	mi;
		BitOutputPort	*	ss;
		int					delay;

		BOOL	diskKeyValid;
		BOOL	titleKeyValid;
		DWORD	titleKeySector;

		BYTE buffer[15];

		Error StartTransfer(void);
		Error EndTransfer(void);
		Error TransferByte(BYTE data, BYTE __far & response);

		Error WriteSeq(int reg, int data, ...);
		Error WriteBytes(int reg, int num, BYTE __far * data);
		Error ReadBytes(int reg, int num, BYTE __far * data);
		Error WriteByte(int reg, BYTE data);
		Error ReadByte(int reg, BYTE __far & data);

	public:
		PioneerDecryption(BitOutputPort * sck,
		                  BitInputPort  * mo,
		                  BitOutputPort * mi,
		                  BitOutputPort * ss,
		                  int delay = 3);

		Error DoAuthenticationCommand(MPEG2DVDAuthenticationCommand com, DWORD sector, BYTE __far * key);
		Error Initialize(void);
		Error PacketReset(void);
	};

class ToshibaDecryption : public MPEG2DVDDecryption
	{
	protected:
		ByteOutputPort	*	indexPort;
		ByteInOutPort	*	dataPort;
		ByteOutputPort	*	streamPort;

 		BYTE	reqoMode;
		BYTE	enboMode;
		BOOL	freqDiv16;

		BOOL	diskKeyValid;
		BOOL	titleKeyValid;
		DWORD	titleKeySector;

		Error WriteSeq(int reg, int data, ...);
		Error WriteBytes(int reg, int num, BYTE __far * data);
		Error ReadBytes(int reg, int num, BYTE __far * data);
		Error WriteByte(int reg, BYTE data);
		Error ReadByte(int reg, BYTE __far & data);
	public:
		ToshibaDecryption(ByteOutputPort *	indexPort,
		                  ByteInOutPort 	*	dataPort,
		                  ByteOutputPort	*	streamPort,
		                  BYTE					reqoMode,
		                  BYTE					enboMode,
		                  BOOL					freqDiv16);

		Error DoAuthenticationCommand(MPEG2DVDAuthenticationCommand com, DWORD sector, BYTE __far * key);
		Error Initialize(void);
		Error PacketReset(void);
	};

#endif

#endif
