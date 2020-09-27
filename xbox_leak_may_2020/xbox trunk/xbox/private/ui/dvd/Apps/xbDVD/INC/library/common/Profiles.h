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
// FILE:      library\common\profiles.h
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
// 16.11.1999  Franky   Added some documentation
// 20.04.2000	Martin	Added some more documentation after searching several hours for a bug
//								(I didn't know to call InitProfiles() first before creating a new profile object in NT kernel)


// The Profile classes are used to store configuration data in a tree-like
// structure. The location where profile information is stored depends on
// the operating system; for Win16, it's in .ini files, for W95/W98/NT it's
// in the registry.
// Where the data is stored is hidden from the programmer; he just uses
// the public methods to store or retrieve key/value pairs.
//
// There are 3 types of profile classes:
// GenericProfile - this is the base class
// StaticProfile - it stores its data in a linear array instead of a tree
//                 and is mostly used for storage in ROM areas
// Profile - derived from GenericProfile, mostly used class
//
// All classes are able to create new entries (actual key/value pairs)
// or sections (folders) through their constructors.
//
// The entries that can be read or written are:
// int, long, DWORD, WORD, string, (bool - only under Win32)
// For ReadXXX functions, if the desired key cannot be opened
// you can pass a default value that will be returned instead.
//
// When creating a new profile object, the corresponding
// object is either opened for read/write access (if it exists)
// or is created newly if it didn't yet exist.
//
// All methods of the ReadDirect() and WriteDirect() format
// (only found in the derived classes) are directly reading
// from / writing to the specified location, using absolute
// path names.
// Methods without "Direct" in their name allow a parent entry
// to be named first under which the child entry will be located
// (which means the path is a relative path, starting at the
// location of the current Profile object).
//
// The constructors can be called in one of the following ways:
//
// bla = new Profile(KernelString name)
//   creates a Profile object with <name> as its complete
//   "path", as seen from the root of the profile tree.
//
// bla = new Profile(Profile * parent, KernelString name)
//   creates a Profile object under the given "parent"
//   object in the tree.
//
// bla = new Profile(KernelString main, KernelString section)
//   creates a Profile object under the location
//   "software\viona\<main>\<section>" in the tree.
//   (e.g. "software\viona\Ravisent\2.0")
//
// bla = new Profile(KernelString vendor, KernelString product, KernelString version)
//   creates a Profile object under the location
//   "software\<vendor>\<product>\<version>" in the tree
//   (e.g. "software\SomeoneProducts, Unlimited\Kickstart\40.72")
//
// IMPORTANT when using the Profiles class in kernel mode drivers:
// Before creating a new Profile object (see above) you have to call the static function
//	Profiles::InitProfiles(). And after deleting the Profile object you have to call
// the static function Profiles::CleanupProfiles()!!!

// Parameter: Was bei Aufruf ohne Default?

#ifndef _PROFILES_H_
#define _PROFILES_H_

#include "gnerrors.h"
#include "krnlstr.h"

#define MAX_PROFILE_NAME_LENGTH 40

// error: unable to write to the specified profile
#define GNR_PROFILE_WRITE			MKERR(ERROR, PROFILES, GENERAL, 0x01)

// error: unable to read from the specified profile
#define GNR_PROFILE_READ			MKERR(ERROR, PROFILES, GENERAL, 0x02)

// error: No child profile could be created
#define GNR_PROFILE_NO_LEVEL		MKERR(ERROR, PROFILES, GENERAL, 0x03)

// Entry for the generic profile
class GenericProfile
	{
	public:
		virtual ~GenericProfile(void) {}
		virtual GenericProfile * CreateSection(KernelString section) = 0;

		virtual BOOL IsValid(void) = 0;

		// XXXDirect methods write directly to the absolute location given in "entry"
		// entry is the absolute path name, value is the value to be written.
		// "deflt", where used, is the value to return from a ReadXXX function when
		// the desired entry does not exist. If in such a case no parameter for
		// "deflt" is passed, the value of "value" is not changed.

		virtual Error WriteDirect(KernelString entry, int value) = 0;
		virtual Error ReadDirect(KernelString entry, int __far & value, int deflt) = 0;
		Error ReadDirect(KernelString entry, int __far & value) {return ReadDirect(entry, value, value);}

		virtual Error WriteDirect(KernelString entry, bool value) = 0;
		virtual Error ReadDirect(KernelString entry, bool __far & value, bool deflt) = 0;
		Error ReadDirect(KernelString entry, bool __far & value) {return ReadDirect(entry, value, value);}

		virtual Error ReadDirect(KernelString entry, long __far & value, long deflt) = 0;
		Error ReadDirect(KernelString entry, long __far & value) {return ReadDirect(entry, value, value);}

		// the base parameter determines the base (2, 8, 10, 16..) of the argument that
		// is about to be read or written.
		// This is required because e.g. when writing binary data to text-based profiles
		// (like under Win16), the data has to be encoded in a textual format (ASCII).
		virtual Error WriteDirect(KernelString entry, WORD value, int base) = 0;
		virtual Error ReadDirect(KernelString entry, WORD __far & value, int base, WORD deflt) = 0;
		Error ReadDirect(KernelString entry, WORD __far & value, int base) {return ReadDirect(entry, value, base, value);}

		virtual Error WriteDirect(KernelString entry, DWORD value, int base) = 0;
		virtual Error ReadDirect(KernelString entry, DWORD __far & value, int base, DWORD deflt) = 0;
		Error ReadDirect(KernelString entry, DWORD __far & value, int base) {return ReadDirect(entry, value, base, value);}

		virtual Error WriteDirect(KernelString entry, KernelString value) = 0;
		virtual Error ReadDirect(KernelString entry, KernelString & value, KernelString deflt) = 0;
		Error ReadDirect(KernelString entry, KernelString & value) {return ReadDirect(entry, value, value);}

		// These Write/Read functions use "section" as relative path, starting at
		// the location of "this" object.
		virtual Error Write(KernelString section, KernelString entry, int value);
		virtual Error Read(KernelString section, KernelString entry, int __far & value, int deflt);
		Error Read(KernelString section, KernelString entry, int __far & value) {return Read(section, entry, value, value);}

		virtual Error Write(KernelString section, KernelString entry, bool value);
		virtual Error Read(KernelString section, KernelString entry, bool __far & value, bool deflt);
		Error Read(KernelString section, KernelString entry, bool __far & value) {return Read(section, entry, value, value);}

		virtual Error Read(KernelString section, KernelString entry, long __far & value, long deflt);
		Error Read(KernelString section, KernelString entry, long __far & value) {return Read(section, entry, value, value);}

		virtual Error Write(KernelString section, KernelString entry, DWORD value, int base = 10);
		virtual Error Read(KernelString section, KernelString entry, DWORD __far & value, int base, DWORD deflt);
		Error Read(KernelString section, KernelString entry, DWORD __far & value, int base) {return Read(section, entry, value, base, value);}

		virtual Error Write(KernelString section, KernelString entry, WORD value, int base);
		virtual Error Read(KernelString section, KernelString entry, WORD __far & value, int base, WORD deflt);
		Error Read(KernelString section, KernelString entry, WORD __far & value, int base) {return Read(section, entry, value, base, value);}

		virtual Error Write(KernelString section, KernelString entry, KernelString value);
		virtual Error Read(KernelString section, KernelString entry, KernelString & value, KernelString deflt);
		Error Read(KernelString section, KernelString entry, KernelString & value) {return Read(section, entry, value, value);}
	};


//
// Entry for the static profile
// The location of keys in a static profile is found by
// calculating a hash value from their name.
//

struct ProfileEntry
	{
	DWORD id;
	DWORD	value;
	};

class StaticProfile : public GenericProfile
	{
	private:
		ProfileEntry	*	entries;

	protected:
		BOOL	protect;

		virtual Error WriteDW(KernelString entry, DWORD value);
		virtual Error ReadDW(KernelString entry, DWORD __far & value);

		virtual Error FindValue(KernelString entryName, ProfileEntry * __far & entry);
		virtual Error FindSection(KernelString section, ProfileEntry * __far & entry);

	public:
		// ProfileEntry array must be initialized (formatted).
		StaticProfile(ProfileEntry * entries);
		StaticProfile(const ProfileEntry * entries);	// Read only profile
		// Start with a subsection.
		StaticProfile(ProfileEntry * entries, KernelString section);
		StaticProfile(StaticProfile * parent, KernelString section);

		virtual void SetWriteProtection(BOOL protect) {this->protect = protect;}	// Default is ON!

		GenericProfile * CreateSection(KernelString section); // creates a new subsection (folder)

		BOOL IsValid(void);

		Error CalculateByteSize (int & size);   // of the valid ProfileEntry array

		// XXXDirect methods write directly to the absolute location given in "entry".
		Error WriteDirect(KernelString entry, int value);
		Error ReadDirect(KernelString entry, int __far & value, int deflt);

		Error WriteDirect(KernelString entry, long value);
		Error ReadDirect(KernelString entry, long __far & value, long deflt);

		Error WriteDirect(KernelString entry, WORD value, int base);
		Error ReadDirect(KernelString entry, WORD __far & value, int base, WORD deflt);

		Error WriteDirect(KernelString entry, DWORD value, int base);
		Error ReadDirect(KernelString entry, DWORD __far & value, int base, DWORD deflt);

		Error WriteDirect(KernelString entry, KernelString value);
		Error ReadDirect(KernelString entry, KernelString & value, KernelString deflt);

		Error WriteDirect(KernelString entry, bool value);
		Error ReadDirect(KernelString entry, bool __far & value, bool deflt);
	};

//
// Entry for the most-used standard profile
// When compiling for Win16, profiles get written to .ini files
// When compiling a standard Win32 app, a VxD or for NT, profiles
// get written into the registry.
//

class NullProfile : public GenericProfile
	{
	public:
		GenericProfile * CreateSection(KernelString section) {return this;}

		BOOL IsValid(void) {return true;}

		Error WriteDirect(KernelString entry, int value) {GNRAISE_OK;}
		Error ReadDirect(KernelString entry, int __far & value, int deflt) {value = deflt; GNRAISE_OK;}

		Error WriteDirect(KernelString entry, bool value)  {GNRAISE_OK;}
		Error ReadDirect(KernelString entry, bool __far & value, bool deflt)  {value = deflt; GNRAISE_OK;}

		Error ReadDirect(KernelString entry, long __far & value, long deflt)  {value = deflt; GNRAISE_OK;}

		Error WriteDirect(KernelString entry, WORD value, int base)  {GNRAISE_OK;}
		Error ReadDirect(KernelString entry, WORD __far & value, int base, WORD deflt)  {value = deflt; GNRAISE_OK;}

		Error WriteDirect(KernelString entry, DWORD value, int base)  {GNRAISE_OK;}
		Error ReadDirect(KernelString entry, DWORD __far & value, int base, DWORD deflt)  {value = deflt; GNRAISE_OK;}

		Error WriteDirect(KernelString entry, KernelString value)  {GNRAISE_OK;}
		Error ReadDirect(KernelString entry, KernelString & value, KernelString deflt)  {value = deflt; GNRAISE_OK;}
	};

#endif
