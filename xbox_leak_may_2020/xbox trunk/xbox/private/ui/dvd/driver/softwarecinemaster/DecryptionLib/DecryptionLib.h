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

#ifndef DECRYPTIONLIB_H
#define DECRYPTIONLIB_H

#define VIONA_VERSION			1
#undef ONLY_EXTERNAL_VISIBLE

#include "library\common\prelude.h"
#include "..\common\prelude.h"
#include "library\common\gnerrors.h"
#include "library\common\tags.h"
#include "library\common\tagunits.h"
#include "library\common\hooks.h"
#include "library\common\profiles.h"
#include "library\common\vddebug.h"

#define ONLY_EXTERNAL_VISIBLE	1

#include "library\hardware\videodma\generic\pip.h"
#include "library\hardware\mpeg2dec\generic\mpeg2dec.h"
#include "..\DVDStreamDecoder.h"

class AuthenticatingDecryptor : public GenericDecryptor
	{
	public:
		AuthenticatingDecryptor(void) {}
		virtual ~AuthenticatingDecryptor(void) {}

		virtual Error DoAuthenticationCommand(MPEG2DVDAuthenticationCommand com, DWORD sector, BYTE __far * key) = 0;
	};

AuthenticatingDecryptor * _fastcall CreateDecryptor(void);

void _fastcall InitDecryption(void);

#endif
