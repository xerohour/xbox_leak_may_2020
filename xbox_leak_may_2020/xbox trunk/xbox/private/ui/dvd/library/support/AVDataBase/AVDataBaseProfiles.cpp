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

#include "AVDataBaseProfiles.h"

AVDataBase::AVDataBase(KernelString swcmcfgdll, KernelString product, KernelString version, int displayDeviceID, int soundDeviceID) :
	legacyProfile("RAVISENT", product, version)
	{
	this->displayDeviceID = displayDeviceID;
	this->soundDeviceID = soundDeviceID;
	openCount = 1;

	// First try to find the specified config library
	dataBaseInstance = ::LoadLibrary(swcmcfgdll);
	if (NULL == dataBaseInstance)
		{
		// Try previous dll name
		dataBaseInstance = ::LoadLibrary("SWCMCFG.dll");
		}
	if (dataBaseInstance)
		{
		(FARPROC &)DBInitialize = GetProcAddress(dataBaseInstance, "_SWCMCFG_Initialize@0");
		(FARPROC &)OpenDB       = GetProcAddress(dataBaseInstance, "_SWCMCFG_OpenDB@4");
		(FARPROC &)CloseDB      = GetProcAddress(dataBaseInstance, "_SWCMCFG_CloseDB@4");
		(FARPROC &)GetDBInt     = GetProcAddress(dataBaseInstance, "_SWCMCFG_GetInt@24");

		if (DBInitialize && OpenDB && CloseDB && GetDBInt)
			{
			if (DBInitialize() != 0)
				{
				dataBase = OpenDB(product + __TEXT("\\") + version);
				if (dataBase == 0)
					{
					::FreeLibrary(dataBaseInstance);
					dataBaseInstance = NULL;
					}
				}
			else
				{
				::FreeLibrary(dataBaseInstance);
				dataBaseInstance = NULL;
				}
			}
		else
			{
			::FreeLibrary(dataBaseInstance);
			dataBaseInstance = NULL;
			}
		}
	}

AVDataBase::~AVDataBase(void)
	{
	if (dataBaseInstance)
		::FreeLibrary(dataBaseInstance);
	}

Error AVDataBase::GetInt(KernelString name, int & value, int deflt)
	{
	if (dataBaseInstance)
		{
		if (GetDBInt(dataBase, name, deflt, displayDeviceID, soundDeviceID, value) != 0)
			GNRAISE_OK;
		else
			GNRAISE(GNR_PROFILE_READ);
		}
	else
		GNRAISE(GNR_PROFILE_READ);
	}

Error AVDataBase::GetString(KernelString name, KernelString & value, KernelString deflt)
	{
	GNREASSERT(legacyProfile.Read("DevDB", name, value, deflt));

	GNRAISE_OK;
	}


AVDataBaseProfile::AVDataBaseProfile(KernelString swcmcfgdll, KernelString product, KernelString version, int displayDevice, int soundDevice)
	{
	mainSection = "";
	avDataBase = new AVDataBase(swcmcfgdll, product, version, displayDevice, soundDevice);
	}

AVDataBaseProfile::AVDataBaseProfile(AVDataBaseProfile * parent, KernelString section)
	{
	mainSection = parent->mainSection + section + ".";
	avDataBase = parent->avDataBase;
	avDataBase->Obtain();
	}

AVDataBaseProfile::~AVDataBaseProfile(void)
	{
	avDataBase->Release();
	}


Error AVDataBaseProfile::WriteDirect(KernelString entry, int value)
	{
	GNRAISE_OK;
	}

Error AVDataBaseProfile::ReadDirect(KernelString entry, int __far & value, int deflt)
	{
	value = deflt;

	GNREASSERT(avDataBase->GetInt(mainSection + entry, value, deflt));

	GNRAISE_OK;
	}

Error AVDataBaseProfile::WriteDirect(KernelString entry, bool value)
	{
	GNRAISE_OK;
	}

Error AVDataBaseProfile::ReadDirect(KernelString entry, bool __far & value, bool deflt)
	{
	int iv;

	value = deflt;

	GNREASSERT(avDataBase->GetInt(mainSection + entry, iv, deflt));

	value = iv != 0;

	GNRAISE_OK;
	}

Error AVDataBaseProfile::ReadDirect(KernelString entry, long __far & value, long deflt)
	{
	int iv;

	value = deflt;

	GNREASSERT(avDataBase->GetInt(mainSection + entry, iv, deflt));

	value = (long)iv;

	GNRAISE_OK;
	}

Error AVDataBaseProfile::WriteDirect(KernelString entry, WORD value, int base)
	{
	GNRAISE_OK;
	}

Error AVDataBaseProfile::ReadDirect(KernelString entry, WORD __far & value, int base, WORD deflt)
	{
	int iv;

	value = deflt;

	GNREASSERT(avDataBase->GetInt(mainSection + entry, iv, deflt));

	value = (WORD)iv;

	GNRAISE_OK;
	}

Error AVDataBaseProfile::WriteDirect(KernelString entry, DWORD value, int base)
	{
	GNRAISE_OK;
	}

Error AVDataBaseProfile::ReadDirect(KernelString entry, DWORD __far & value, int base, DWORD deflt)
	{
	int iv;

	value = deflt;

	GNREASSERT(avDataBase->GetInt(mainSection + entry, iv, deflt));

	value = (DWORD)iv;

	GNRAISE_OK;
	}

Error AVDataBaseProfile::WriteDirect(KernelString entry, KernelString value)
	{
	GNRAISE_OK;
	}

Error AVDataBaseProfile::ReadDirect(KernelString entry, KernelString & value, KernelString deflt)
	{
	value = deflt;

	GNREASSERT(avDataBase->GetString(mainSection + entry, value, deflt));

	GNRAISE_OK;
	}
