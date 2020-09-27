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

//
// FILE:      library\common\profiles.cpp
// AUTHOR:    S. Herr
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   28.11.95
//
// PURPOSE:   Classes for configuration (INI) files.
//
// HISTORY:
//
// Date			Author	Comment
// --------		------	-------

#include "library/common/profiles.h"
#include "library/lowlevel/timer.h"
#include "library/common/vddebug.h"

///////////////////////////////////////////////////////////////////////////////
// Generic Profile Implementation
///////////////////////////////////////////////////////////////////////////////

Error GenericProfile::Write(KernelString section, KernelString entry, int value)
	{
	Error err;

	GenericProfile * child = CreateSection(section);
	if (child)
		{
		err = child->WriteDirect(entry, value);
		delete child;
		}
	else
		err = GNR_PROFILE_NO_LEVEL;

	GNRAISE(err);
	}

Error GenericProfile::Read(KernelString section, KernelString entry, int __far & value, int deflt)
	{
	Error err;

	GenericProfile * child = CreateSection(section);
	if (child)
		{
		err = child->ReadDirect(entry, value, deflt);
		delete child;
		}
	else
		err = GNR_PROFILE_NO_LEVEL;

	GNRAISE(err);
	}

Error GenericProfile::Write(KernelString section, KernelString entry, bool value)
	{
	Error err;

	GenericProfile * child = CreateSection(section);
	if (child)
		{
		err = child->WriteDirect(entry, value);
		delete child;
		}
	else
		err = GNR_PROFILE_NO_LEVEL;

	GNRAISE(err);
	}

Error GenericProfile::Read(KernelString section, KernelString entry, bool __far & value, bool deflt)
	{
	Error err;

	GenericProfile * child = CreateSection(section);
	if (child)
		{
		err = child->ReadDirect(entry, value, deflt);
		delete child;
		}
	else
		err = GNR_PROFILE_NO_LEVEL;

	GNRAISE(err);
	}


Error GenericProfile::Read(KernelString section, KernelString entry, long __far & value, long deflt)
	{
	Error err;

	GenericProfile * child = CreateSection(section);
	if (child)
		{
		err = child->ReadDirect(entry, value, deflt);
		delete child;
		}
	else
		err = GNR_PROFILE_NO_LEVEL;

	GNRAISE(err);
	}

Error GenericProfile::Write(KernelString section, KernelString entry, DWORD value, int base)
	{
	Error err;

	GenericProfile * child = CreateSection(section);
	if (child)
		{
		err = child->WriteDirect(entry, value, base);
		delete child;
		}
	else
		err = GNR_PROFILE_NO_LEVEL;

	GNRAISE(err);
	}


Error GenericProfile::Read(KernelString section, KernelString entry, DWORD __far & value, int base, DWORD deflt)
	{
	Error err;

	GenericProfile * child = CreateSection(section);
	if (child)
		{
		err = child->ReadDirect(entry, value, base, deflt);
		delete child;
		}
	else
		err = GNR_PROFILE_NO_LEVEL;

	GNRAISE(err);
	}

Error GenericProfile::Write(KernelString section, KernelString entry, WORD value, int base)
	{
	Error err;

	GenericProfile * child = CreateSection(section);
	if (child)
		{
		err = child->WriteDirect(entry, value, base);
		delete child;
		}
	else
		err = GNR_PROFILE_NO_LEVEL;

	GNRAISE(err);
	}


Error GenericProfile::Read(KernelString section, KernelString entry, WORD __far & value, int base, WORD deflt)
	{
	Error err;

	GenericProfile * child = CreateSection(section);
	if (child)
		{
		err = child->ReadDirect(entry, value, base, deflt);
		delete child;
		}
	else
		err = GNR_PROFILE_NO_LEVEL;

	GNRAISE(err);
	}

Error GenericProfile::Write(KernelString section, KernelString entry, KernelString value)
	{
	Error err;

	GenericProfile * child = CreateSection(section);
	if (child)
		{
		err = child->WriteDirect(entry, value);
		delete child;
		}
	else
		err = GNR_PROFILE_NO_LEVEL;

	GNRAISE(err);
	}


Error GenericProfile::Read(KernelString section, KernelString entry, KernelString & value, KernelString deflt)
	{
	Error err;

	GenericProfile * child = CreateSection(section);
	if (child)
		{
		err = child->ReadDirect(entry, value, deflt);
		delete child;
		}
	else
		err = GNR_PROFILE_NO_LEVEL;

	GNRAISE(err);
	}



///////////////////////////////////////////////////////////////////////////////
// Static Profile Implementation
///////////////////////////////////////////////////////////////////////////////


static DWORD CalculateID(BOOL section, KernelString name, int start, int range)
	{
	DWORD temp = 0;

	if (name.Length() > 0)
		{
		for (int i = start; i < start + range; i++)
			{
			temp ^= name[i];
			temp = (temp << 3) | (temp >> 29);	// The first 10 characters count
			}

		return (section ? (temp |= 1) : (temp &= ~1));
		}
	else
		return 0;
	}

static ProfileEntry * WalkSetupInfo(ProfileEntry * entries, int start, DWORD id)
	{
	DWORD i = start + 1;
	DWORD range = entries->value;

	while (i < range)
		{
		if (entries[i].id == id)
			return entries + i;
		else if (entries[i].id & 1)
			i += entries[i].value;
		else
			i++;
		}

	return NULL;
	}

Error StaticProfile::FindValue(KernelString entryName, ProfileEntry * __far & entry)
	{
	DWORD id;

	if (entries && (id = CalculateID(FALSE, entryName, 0, entryName.Length())))
		{
		entry = WalkSetupInfo(entries, 0, id);

		if (entry)
			GNRAISE_OK;
		}

	entry = NULL;
	GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

Error StaticProfile::FindSection(KernelString section, ProfileEntry * __far & entry)
	{
	int start, range;
	DWORD	id;

	ProfileEntry * entryPtr = this->entries;	// Initialize with root

	start = 0;

	while (entryPtr && start < section.Length())
		{
		range = section.Next(__TEXT("\\"), start - 1) - start;	// Next increments its argument!

		id = CalculateID(TRUE, section, start, range);
		entryPtr = WalkSetupInfo(entryPtr, 0, id);	// entryPtr->value - 1: section header is counted for

		start = start + range + 1;	// Skip "\"
		}

	if (entryPtr)
		{
		entry = entryPtr;
		GNRAISE_OK;
		}
	else
		GNRAISE(GNR_OBJECT_NOT_FOUND);

	GNRAISE_OK;
	}

StaticProfile::StaticProfile(ProfileEntry * entries)
	{
	this->protect = FALSE;
	this->entries = entries;
	}

StaticProfile::StaticProfile(const ProfileEntry * entries)
	{
	this->protect = TRUE;
	this->entries = (ProfileEntry *) entries;
	}


StaticProfile::StaticProfile(ProfileEntry * entries, KernelString section)
	{
	ProfileEntry * sectionEntry;

	this->protect = TRUE;

	this->entries = entries;

	// Find section
	if (!IS_ERROR(FindSection(section, sectionEntry)))
		{
		// Now the search will become faster by adapting the offset into the array
		this->entries = sectionEntry;
		}
	}

StaticProfile::StaticProfile(StaticProfile * parent, KernelString section)
	{
	ProfileEntry * sectionEntry;

	this->protect = TRUE;

	if (!IS_ERROR(parent->FindSection(section, sectionEntry)))
		this->entries = sectionEntry;
	else
		this->entries = NULL;
	}

GenericProfile * StaticProfile::CreateSection(KernelString section)
	{
	StaticProfile *profile = new StaticProfile(this, section);
	profile->protect = this->protect;   // clone protection setting
	return profile;
	}

BOOL StaticProfile::IsValid(void)
	{
	return TRUE;
	}

Error StaticProfile::CalculateByteSize (int & size)
	{
	// The top section has the number of ProfileEntry entries.
	size = entries->value * sizeof(ProfileEntry);
	GNRAISE_OK;
	}

Error StaticProfile::WriteDW(KernelString entry, DWORD value)
	{
	ProfileEntry * valEntry;

	if (protect)
		GNRAISE(GNR_OBJECT_READ_ONLY);
	else
		{
		if (!IS_ERROR(FindValue(entry, valEntry)))
			valEntry->value = value;
		else
			GNRAISE(GNR_OBJECT_NOT_FOUND);
		}

	GNRAISE_OK;
	}

Error StaticProfile::ReadDW(KernelString entry, DWORD __far & value)
	{
	ProfileEntry * valEntry;

	if (!IS_ERROR(FindValue(entry, valEntry)))
		value = valEntry->value;
	else
		GNRAISE(GNR_OBJECT_NOT_FOUND);

	GNRAISE_OK;
	}

Error StaticProfile::WriteDirect(KernelString entry, int value)
	{
	GNRAISE(WriteDW(entry, (DWORD) value));	// Type size platform specific...
	}

Error StaticProfile::WriteDirect(KernelString entry, long value)
	{
	GNRAISE(WriteDW(entry, (DWORD) value));
	}

Error StaticProfile::WriteDirect(KernelString entry, WORD value, int base)
	{
	GNRAISE(WriteDW(entry, ((DWORD) value) & 0xffff0000));
	}

Error StaticProfile::WriteDirect(KernelString entry, DWORD value, int base)
	{
	GNRAISE(WriteDW(entry, value));
	}

Error StaticProfile::ReadDirect(KernelString entry, int __far & value, int deflt)
	{
	DWORD dw;
	Error error;

	if (!IS_ERROR(error = ReadDW(entry, dw)))
		value = (int) dw;
	else
		value = deflt;

	GNRAISE(error);
	}

Error StaticProfile::ReadDirect(KernelString entry, long __far & value, long deflt)
	{
	DWORD dw;
	Error error;

	if (!IS_ERROR(error = ReadDW(entry, dw)))
		value = (long) dw;
	else
		value = deflt;

	GNRAISE(error);
	}

Error StaticProfile::ReadDirect(KernelString entry, WORD __far & value, int base, WORD deflt)
	{
	DWORD dw;
	Error error;

	if (!IS_ERROR(error = ReadDW(entry, dw)))
		value = (WORD) dw;
	else
		value = deflt;

	GNRAISE(error);
	}

Error StaticProfile::ReadDirect(KernelString entry, DWORD __far & value, int base, DWORD deflt)
	{
	DWORD dw;
	Error error;

	if (!IS_ERROR(error = ReadDW(entry, dw)))
		value = (int) dw;
	else
		value = deflt;

	GNRAISE(error);
	}

Error StaticProfile::WriteDirect(KernelString entry, KernelString value)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

Error StaticProfile::ReadDirect(KernelString entry, KernelString & value, KernelString deflt)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}


Error StaticProfile::WriteDirect(KernelString entry, bool value)
	{
	GNRAISE(WriteDW(entry, value ? 1 : 0));
	}

Error StaticProfile::ReadDirect(KernelString entry, bool __far & value, bool deflt)
	{
	DWORD dw;
	Error error;

	if (!IS_ERROR(error = ReadDW(entry, dw)))
		value = (bool) (dw != 0);
	else
		value = deflt;

	GNRAISE(error);
	}




