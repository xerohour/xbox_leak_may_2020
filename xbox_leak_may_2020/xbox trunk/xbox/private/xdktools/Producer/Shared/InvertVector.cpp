#include <stdafx.h>
#include "InvertVector.h"

void InvertVector::FindBitPos()
{
	int i;
	int nextinversion = 0;
	highestChordMember = -1;
	for(i = 0; i < MAX_INVERT+1; i++)
	{
		ChordMemberBitPos[i] = -1;
	}
	for(i = 0; i < 24 && nextinversion < MAX_INVERT+1; i++)
	{
		if(ulChord & (1 << i))
		{
			ChordMemberBitPos[nextinversion++] = i;
			++highestChordMember;
		}
	}
}

void InvertVector::ComputeDisallowedMasks()
{
	FindBitPos();
	for(int i = 0; i < highestChordMember; i++)
	{
		DisallowedMasks[i] = 0x00000FFF;
		for(int j = ChordMemberBitPos[i]; j <= ChordMemberBitPos[i+1]; j++)
		{
			DisallowedMasks[i] &= ~(1 << j);
		}
		unsigned long tmpmask = DisallowedMasks[i] << 12;
		DisallowedMasks[i] |= tmpmask;
	}
}


InvertVector::InvertVector(unsigned long chord, unsigned long invert)
: ulInvert(invert), ulChord(chord)
{
	ComputeDisallowedMasks();
}

void InvertVector::SetAllowedInversion(int inversion, bool allowed)
{
	// assumes that InvertBitPos, bAllowed, ulChord, and ulInvert set
	// changes ulInvert and bAllowed to conform to other settings
//	ASSERT(inversion > - 1 && inversion < highestChordMember);
	if(inversion < 0 || inversion >= highestChordMember)
	{
		return;
	}
	else if(ChordMemberBitPos[inversion] < 0 || ChordMemberBitPos[inversion+1] < 0)
	{
		return;
	}
	if(allowed)
	{
		bool Allowed[3];
		int i;
		for(i = 0; i < highestChordMember; i++)
		{
			if(i == inversion)
			{
				Allowed[i] = true;;
			}
			else
			{
				Allowed[i] = GetAllowedInversion(i);
			}
		}
		ulInvert = 0xFFFFFF;
		for(i = 0; i < highestChordMember; i++)
		{
			if(!Allowed[i])
				ulInvert &= DisallowedMasks[i];
		}
	}
	else
	{
		ulInvert &= DisallowedMasks[inversion];
	}
}

void InvertVector::SetChord(unsigned long chord)
{
	// need to preserve allowed inversion info
	bool Allowed[3] = {false, false, false};
	int i;
	for(i = 0; i < highestChordMember; i++)
	{
		Allowed[i] = GetAllowedInversion(i);
	}

	ulChord = chord;
	// now recompute masks and set invert vector according to perserved inversion info
	ComputeDisallowedMasks();
	ulInvert = 0xFFFFFF;
	for(i = 0; i < highestChordMember; i++)
	{
		if(!Allowed[i])
		{
			ulInvert &= DisallowedMasks[i];
		}
	}
}
