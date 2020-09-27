////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000 Ravisent Technologies, Inc.  All Rights Reserved.           //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of Ravisent Technologies, Inc.    //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between Ravisent and Microsoft.  This            //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by Ravisent.  THE ONLY PERSONS   //
// WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS                     //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO RAVISENT, IN THE FORM                 //
// PRESCRIBED BY RAVISENT.                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef AVDATABASEPROFILES_H
#define AVDATABASEPROFILES_H

#include "library\common\profiles.h"
#include "swcmcfg.h"

class AVDataBase
	{
	private:
		int			openCount;

		HCFGDB		dataBase;
		HINSTANCE	dataBaseInstance;

		int			displayDeviceID;
		int			soundDeviceID;

		HCFGDB (WINAPI* OpenDB)(LPCTSTR);
		int	 (WINAPI* DBInitialize)();
		int	 (WINAPI* CloseDB)(HCFGDB);
		int	 (WINAPI* GetDBInt)(HCFGDB, LPCTSTR, int, int, int, int&);

		Profile		legacyProfile;
	public:
		AVDataBase(KernelString swcmcfgdll, KernelString product, KernelString version, int displayDeviceID = 1, int soundDeviceID = 1);
		~AVDataBase(void);

		BOOL IsValid(void) {return dataBaseInstance != NULL;}

		void Obtain(void) {openCount++;}
		void Release(void) {openCount--; if (!openCount) delete this;}

		Error GetInt(KernelString name, int & value, int deflt);
		Error GetString(KernelString name, KernelString & value, KernelString deflt);
	};


class AVDataBaseProfile : public GenericProfile
	{
	private:
		KernelString	mainSection;
		AVDataBase	*	avDataBase;
	public:
		AVDataBaseProfile(KernelString swcmcfgdll, KernelString product, KernelString version, int displayDevice = 1, int soundDevice = 1);
		AVDataBaseProfile(AVDataBaseProfile * parent, KernelString section);
		~AVDataBaseProfile(void);

		BOOL IsValid(void) {return avDataBase->IsValid();}
		GenericProfile * CreateSection(KernelString section) {return new AVDataBaseProfile(this, section);}

		Error WriteDirect(KernelString entry, int value);
		Error ReadDirect(KernelString entry, int __far & value, int deflt);
		Error ReadDirect(KernelString entry, int __far & value) {return ReadDirect(entry, value, value);}

		Error WriteDirect(KernelString entry, bool value);
		Error ReadDirect(KernelString entry, bool __far & value, bool deflt);
		Error ReadDirect(KernelString entry, bool __far & value) {return ReadDirect(entry, value, value);}

		Error ReadDirect(KernelString entry, long __far & value, long deflt);
		Error ReadDirect(KernelString entry, long __far & value) {return ReadDirect(entry, value, value);}

		Error WriteDirect(KernelString entry, WORD value, int base);
		Error ReadDirect(KernelString entry, WORD __far & value, int base, WORD deflt);
		Error ReadDirect(KernelString entry, WORD __far & value, int base) {return ReadDirect(entry, value, base, value);}

		Error WriteDirect(KernelString entry, DWORD value, int base);
		Error ReadDirect(KernelString entry, DWORD __far & value, int base, DWORD deflt);
		Error ReadDirect(KernelString entry, DWORD __far & value, int base) {return ReadDirect(entry, value, base, value);}

		Error WriteDirect(KernelString entry, KernelString value);
		Error ReadDirect(KernelString entry, KernelString & value, KernelString deflt);
		Error ReadDirect(KernelString entry, KernelString & value) {return ReadDirect(entry, value, value);}
	};

#endif
