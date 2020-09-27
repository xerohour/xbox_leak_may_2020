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

#include "DiskItemName.h"

//
//  Normalize pathname, i.e. set all separator signs to default separator
//

void DiskItemName::Normalize(void)
	{
	int read = 0;
	int len = name.Length();
	KernelString help = "";

	while (read < len)
		{
		//
		//  make sure we only have the default separator
		//

		if (separators.Contains(name[read]))
			{
			name[read] = GetDefaultSeparator();

			//
			//  Remove double separators
			//

			if (read > 0 && name[read - 1] == GetDefaultSeparator())
				{
				read++;
				continue;
				}
			}

		help += name[read];
		read++;
		}

	name = help;
	}

//
//  Remove first path component
//

Error DiskItemName::RemoveFrontComp(void)
	{
	int pos = name.First(GetDefaultSeparator());

	if (!pos)
		{
		pos = name.Next(GetDefaultSeparator(), 0);
		if (pos >= 0)
			name = name.Seg(pos + 1, name.Length() - pos - 1);
		else
			name = name.Seg(1, name.Length() - 1);
		}
	else if (pos > 0)
		name = name.Seg(pos + 1, name.Length() - pos - 1);

	GNRAISE_OK;
	}

//
//  Remove last path component
//

Error DiskItemName::RemoveEndComp(void)
	{
	int pos1 = name.Last(GetDefaultSeparator());
	int pos2;

	if (pos1 >= 0)
		{
		pos2 = name.Prev(GetDefaultSeparator(), pos1);

		if (pos2 >= 0)
			name = name.Head(pos2) + name.Tail(name.Length() - pos1);
		else
			name = name.Tail(name.Length() - pos1 - 1);
		}

	GNRAISE_OK;
	}

//
//  First component of path
//

DiskItemName DiskItemName::GetFront(void) const
	{
	int pos = name.First(GetDefaultSeparator());

	if (!pos)
		{
		pos = name.Next(GetDefaultSeparator(), 0);
		if (pos >= 0)
			return name.Seg(1, pos - 1);
		else
			return name.Seg(1, name.Length() - 1);
		}
	else if (pos > 0)
		return name.Head(pos);
	else
		return "";
	}

//
//  Get path without first component
//

DiskItemName DiskItemName::GetTail(void) const
	{
	int pos = name.First(GetDefaultSeparator());

	if (pos >= 0)
		return name.Tail(name.Length() - pos - 1);
	else
		return name;
	}

//
//  File name without extension
//

DiskItemName DiskItemName::GetBaseName(void) const
	{
	int pos1 = name.Last(GetDefaultSeparator());
	int pos2;

	if (pos1 >= 0)
		{
		pos2 = name.Last('.');
		if (pos2 > pos1)	// Excludes case when not found
			return name.Seg(pos2 + 1, pos2 - pos1);
		else
			return name.Tail(name.Length() - pos1 - 1);
		}
	else
		{
		//
		// File name only
		//

		pos2 = name.Last('.');
		if (pos2 > 0)
			return name.Head(pos2);
		else
			return name;
		}
	}

//
//  Get file extension (without '.')
//

DiskItemName DiskItemName::GetFileExt(void) const
	{
	int pos1 = name.Last('.');
	int pos2 = name.Last(GetDefaultSeparator());

	if (pos1 > pos2)
		return name.Tail(name.Length() - pos1 - 1);
	else
		return "";
	}

//
//  Concatenate two item names
//  If first one contains a file name this gets lost
//

DiskItemName operator+(const DiskItemName & u, const DiskItemName & v)
	{
	return DiskItemName(u.name + (KernelString)u.GetDefaultSeparator() + v.name);
	}

//
//  Add item name
//  If "this" has a file name this gets lost
//

DiskItemName & DiskItemName::operator+= (const DiskItemName & u)
	{
	name += GetDefaultSeparator() + u.name;
	Normalize();
	return *this;
	}

//
//  Check if name matches a given mask
//  Supported are * and ?
//

BOOL DiskItemName::Matches(const DiskItemName & mask, DWORD flags) const
	{
	return Matches(mask, 0, 0, flags);
	}

//
//  Internal match function
//

BOOL DiskItemName::Matches(const DiskItemName & mask, int maskRead, int nameRead, DWORD flags) const
	{
	int starPos = mask.name.Next((KernelString)'*', maskRead - 1);
	int segEnd;

	if (maskRead == mask.Length())
		{
		return nameRead == name.Length();
		}
	else
		{
		//
		//  If there is some mask left, but no name, then mask may only contains '*'s
		//

		if (nameRead == name.Length())
			{
			while (maskRead < mask.Length())
				{
				if (mask[maskRead] != '*')
					return FALSE;
				maskRead++;
				}
			return TRUE;
			}
		}

	//
	//  Treat wildcard *
	//

	if (starPos == maskRead)
		{
		segEnd = mask.name.Next((TCHAR)'*', maskRead);
		if (segEnd >= 0)
			{
			KernelString seg;
			int segPos;

			seg = mask.name.Seg(maskRead + 1, segEnd - maskRead - 1);
			segPos = name.Next(seg, nameRead);
			while (segPos < name.Length())
				{
				if (Matches(mask, maskRead + 1 + seg.Length(), segPos + seg.Length(), flags))
					return TRUE;

				segPos = name.Next(seg, segPos);
				}

			return FALSE;
			}
		else
			{
			//
			//  If there is only one wildcard * then the tails must match
			//

			if (maskRead == mask.Length() - 1)	// Star at the end matches everything
				return TRUE;
			else
				{
				if (name.Length() - nameRead < mask.Length() - maskRead - 1)
					return FALSE;
				else
					return MatchChar(mask, maskRead + 1, name.Length() - (mask.Length() - maskRead - 1),
										  mask.Length() - maskRead - 1, flags);
				}
			}
		}
	else if (starPos > maskRead)
		{
		return MatchChar(mask, maskRead, nameRead, starPos - maskRead, flags) &&
				 Matches(mask, starPos, nameRead + starPos - maskRead, flags);
		}
	else
		{
		//
		//  When there is no star wildcard just match the tails of the strings
		//

		if (mask.Length() - maskRead == name.Length() - nameRead)
			return MatchChar(mask, maskRead, nameRead, mask.Length() - maskRead, flags);
		else
			return FALSE;	// Length of tails does not match
		}
	}

//
//  Match sequence of chars including ('?')
//  Note that maskRead + len must be inside mask string
//

#define DIN_CAPS(x)	(x >='a' && x <= 'z' ? x - 'a' + 'A' : x)

BOOL DiskItemName::MatchChar(const DiskItemName & mask, int maskRead, int nameRead, int len, DWORD flags) const
	{
	int hlen = len;

	while (hlen && nameRead < name.Length())
		{
		if (mask[maskRead] !='?')
			{
			if (flags & DINM_CASE_SENSITIVE)
				{
				if (mask[maskRead] != name[nameRead])
					return FALSE;
				}
			else
				{
				if (DIN_CAPS(mask[maskRead]) != DIN_CAPS(name[nameRead]))
					return FALSE;
				}
			}

		maskRead++;
		nameRead++;
		hlen--;
		}

	//
	//  If there are characters left to check then we ran out of chars in name -> not equal

	if (hlen)
		return FALSE;
	else
		return TRUE;
	}
