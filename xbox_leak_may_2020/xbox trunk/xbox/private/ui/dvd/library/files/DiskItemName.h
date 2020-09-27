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

////////////////////////////////////////////////////////////////////
//
//  Disk Item Name Handling
//
////////////////////////////////////////////////////////////////////

#ifndef DISKITEMNAME_H
#define DISKITEMNAME_H

#include "library/common/krnlstr.h"

//
//  Flags for DiskItemName::Matches()
//

#define DINM_NONE					0x00000000		// Nothing at all
#define DINM_CASE_SENSITIVE	0x00000001		// Match must be case sensitive

////////////////////////////////////////////////////////////////////
//
//  Disk Item Name Class
//  A disk item name consists of a path and a file name
//
////////////////////////////////////////////////////////////////////

class DiskItemName
	{
	protected:
		KernelString name;								// The name
		KernelString separators;						// Allowed separators for path components

		void Normalize(void);							// Normalize path so it only has default separators
		BOOL Matches(const DiskItemName & mask, int maskRead, int nameRead, DWORD flags) const;
		BOOL MatchChar(const DiskItemName & mask, int maskRead, int nameRead, int len, DWORD flags) const;

	public:
		DiskItemName(void);									// Create empty path name
		DiskItemName(const TCHAR c);						// Create path name from single character (e.g. drive letter)
		DiskItemName(const TCHAR * name);				// Create path name
		DiskItemName(const TCHAR * name, int len);	// Create path name from not '0' terminated string
		DiskItemName(const KernelString & name);		// Create path name
		DiskItemName(const DiskItemName & name);		// Copy constructor
		virtual ~DiskItemName(void);

		//
		//  Separator handling
		//

		void			 SetSeparators(const KernelString & sep);	// Set allowed separators for path components
		KernelString GetSeparators(void) const;					// Get allowed separators for path components
		void			 SetDefaultSeparator(TCHAR c);				// Set the default separator
		TCHAR			 GetDefaultSeparator(void) const;			// Get the default separator

		//
		//  Construct name
		//

		Error AddFrontComp(const KernelString & comp);		// Add path component to front of path
		Error AddEndComp(const KernelString & comp);			// Add path component to end of path
		Error RemoveFrontComp(void);								// Remove first path component
		Error RemoveEndComp(void);									// Remove last path component
		void	SetToRoot(void);										// Set path to root path and clear file name
		Error SetPathName(const KernelString & name);		// Set path name only
		Error SetFileName(const KernelString & name);		// Set complete file name (including extension)
		Error SetFileExt(const KernelString & ext);			// Set extension only (ext shall not contain '.')

		//
		//  Get path name/components
		//

		DiskItemName GetPath(void) const;					// Path name without file name
		DiskItemName GetFront(void) const;					// First component of path
		DiskItemName GetTail(void) const;					// Get path without first component

		DiskItemName GetFileName(void) const;				// File name with extension
		DiskItemName GetBaseName(void) const;				// File name without extension
		DiskItemName GetFileExt(void) const;				// File extension

		DiskItemName GetPathAndBaseName(void) const;		// Path and file name without extension

		//
		//  Misc. inquiry
		//

		BOOL IsPath(void) const;								// Test if item name points to path or file
		BOOL IsFileOnly(void) const;							// Test if item name has no path components
		BOOL IsRelative(void) const;							// Test if path name is relative (leading separator)
		BOOL IsRoot(void) const;								// Test if item name describes root dir
		BOOL Matches(const DiskItemName & mask, DWORD flags) const;	// Test if item name matches a given mask
		int  Length(void) const;

		//
		//  Operators
		//

		friend DiskItemName operator+(const DiskItemName & u, const DiskItemName & v);
		DiskItemName & operator+= (const DiskItemName & u);
		DiskItemName & operator=(const KernelString & str);

		friend BOOL operator==(const DiskItemName & u, const DiskItemName & v) { return u.name == v.name; }
		friend BOOL operator!=(const DiskItemName & u, const DiskItemName & v) { return u.name != v.name; }

		TCHAR & operator[] (const int index);
		const TCHAR & operator[] (const int index) const;

		operator KernelString * (void);
		operator TCHAR * (void);
	};

//
//  Default Constructor
//

inline DiskItemName::DiskItemName(void)
	{
	separators = "/\\";
	name = "";
	}

//
//  Constructor by single character
//

inline DiskItemName::DiskItemName(const TCHAR c)
	{
	separators = "/\\";
	this->name = c;
	Normalize();
	}

//
//  Constructor by C string
//

inline DiskItemName::DiskItemName(const TCHAR * name)
	{
	separators = "/\\";
	this->name = name;
	Normalize();
	}

//
//  Constructor by C string with length
//

inline DiskItemName::DiskItemName(const TCHAR * name, int len)
	{
	TCHAR * str = new TCHAR[len + 1];

	separators = "/\\";
	strncpy(str, name, len);
	str[len] = '\0';
	this->name = str;
	delete[] str;
	Normalize();
	}

//
//  Constructor by KernelString
//

inline DiskItemName::DiskItemName(const KernelString & name)
	{
	separators = "/\\";
	this->name = name;
	Normalize();
	}

//
//  Copy constructor
//

inline DiskItemName::DiskItemName(const DiskItemName & din)
	{
	name = din.name;
	separators = din.separators;
	}

//
//  Destructor
//

inline DiskItemName::~DiskItemName(void)
	{
	}

//
//  Set path component separators
//  The first character in the string serves as default separator
//

inline void DiskItemName::SetSeparators(const KernelString & sep)
	{
	separators = sep;
	Normalize();
	}

//
//  Return separators
//

inline KernelString DiskItemName::GetSeparators(void) const
	{
	return separators;
	}

//
//  Set default separator
//

inline void DiskItemName::SetDefaultSeparator(TCHAR c)
	{
	separators[0] = c;
	Normalize();
	}

//
//  Set default separator
//

inline TCHAR DiskItemName::GetDefaultSeparator(void) const
	{
	return separators[0];
	}

//
//  Set path name, leave file component
//

inline Error DiskItemName::SetPathName(const KernelString & pathName)
	{
	name = pathName + (KernelString)GetDefaultSeparator() + GetFileName();
	Normalize();
	GNRAISE_OK;
	}

//
//  Add path component to front of path
//

inline Error DiskItemName::AddFrontComp(const KernelString & comp)
	{
	name = comp + (KernelString)GetDefaultSeparator() + name;
	Normalize();
	GNRAISE_OK;
	}

//
// Add path component to end of path
//

inline Error DiskItemName::AddEndComp(const KernelString & comp)
	{
	name = GetPath() + (KernelString)GetDefaultSeparator() + comp + (KernelString)GetDefaultSeparator() + GetFileName();
	Normalize();
	GNRAISE_OK;
	}

//
//  Set path to root dir and delete file name
//

inline void DiskItemName::SetToRoot(void)
	{
	name = KernelString(GetDefaultSeparator());
	}

//
//  Set complete name (incl. path and file)
//

inline DiskItemName & DiskItemName::operator=(const KernelString & name)
	{
	this->name = name;
	Normalize();
	return *this;
	}

//
//  Set file name (including extension)
//

inline Error DiskItemName::SetFileName(const KernelString & name)
	{
	this->name = GetPath() + name;
	GNRAISE_OK;
	}

//
//  Set file extension only (ext shall not contain '.')
//

inline Error DiskItemName::SetFileExt(const KernelString & ext)
	{
	int pos = name.Last('.');

	if (pos > 0)
		name = name.Seg(0, pos + 1) + ext;
	else
		name += "." + ext;

	GNRAISE_OK;
	}

//
//  Get path (without file name)
//

inline DiskItemName DiskItemName::GetPath(void) const
	{
	int pos = name.Last(GetDefaultSeparator());

	if (pos >= 0)
		return DiskItemName(name.Seg(0, pos));
	else
		return DiskItemName("");
	}

//
//  File name with extension
//

inline DiskItemName DiskItemName::GetFileName(void) const
	{
	int pos = name.Last(GetDefaultSeparator());

	if (pos >= 0)
		return DiskItemName(name.Seg(pos + 1, name.Length() - pos - 1));
	else
		return DiskItemName("");
	}

//
//  Get path and file name without extension
//

inline DiskItemName DiskItemName::GetPathAndBaseName(void) const
	{
	int pos = name.Last('.');

	if (pos >= 0)
		return DiskItemName(name.Seg(0, pos));
	else
		return DiskItemName(name);
	}

//
//  Get length
//

inline int DiskItemName::Length(void) const
	{
	return name.Length();
	}

//
//  Test if item name points to path or file
//

inline BOOL DiskItemName::IsPath(void) const
	{
	return name[name.Length() - 1] == GetDefaultSeparator();
	}

//
//  Test if item name has no path components
//

inline BOOL DiskItemName::IsFileOnly(void) const
	{
	return name.First(GetDefaultSeparator()) > 0;
	}

//
//  Test if path name is relative (leading separator)
//

inline BOOL DiskItemName::IsRelative(void) const
	{
	return IsFileOnly() || name[0] != GetDefaultSeparator();
	}

//
//  Test if item name describes root dir
//

inline BOOL DiskItemName::IsRoot(void) const
	{
	return name == GetDefaultSeparator();
	}

//
//  Convert to TCHAR string
//

inline DiskItemName::operator TCHAR * (void)
	{
	return (char*)name;
	}

//
//  Convert to KernelString
//

inline DiskItemName::operator KernelString * (void)
	{
	return &name;
	}

//
//  Operator []
//

inline TCHAR & DiskItemName::operator[] (const int index)
	{
	return name[index];
	}

//
//  Operator []
//

inline const TCHAR & DiskItemName::operator[] (const int index) const
	{
	return name[index];
	}

#endif
