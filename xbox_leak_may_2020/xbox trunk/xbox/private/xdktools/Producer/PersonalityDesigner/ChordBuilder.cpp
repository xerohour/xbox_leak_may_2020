#include "stdafx.h"
#include <string.h>
#include "chordbuilder.h"

/*  
Rules for deducing the diatonic chords in a scale.

  Let scale = scale pattern(24 bits);
  Let root = root of scale (0 - 11);

  if(root != 0 mod 12) then scale = rotate(scale, -root);

  Assert (0x1 & scale);	// root is defined

	Let NthOne(X, n) = bit pos of nth '1' in X. Bit pos is equivalent to chromatic steps
	Thus if X = 0x5B (01011011), NthOne(X,0) = -1 (all X)
								 NthOne(X,1) = 0
								 NthOne(X,2) = 1
								 NthOne(X,3) = 3
								 NthOne(X,4) = 4
								 NthOne(X,5) = 6
								 NthOne(X,n) = -1 n > 6

	Third(scale) =  NthOne(scale, 3)	// third of chord
	Fifth(scale) = NthOne(scale, 5)		// fifth of chord
	Seventh(scale) = NthOne(scale, 7)	// seventh of chord

	// classifying intervals:

	Thirds:	2 = dim, 3 = min, 4 = maj, 5 = aug
	Fifths: 6 = dim, 7 = Per, 8 = aug
	Sevenths 9 = dim, 10 = min, 11 = major

	// classifying triads (parenthesis show root relative to unison, eg, if C is unison, then
	// bVI 63 means the a flat chord in 1st inversion):

	3rd \ 5th  dim		per		aug

	dim			b5sus2	sus2	#5sus2
	min			dim		min		63	(bVI 63)
	maj			b5		maj		aug
	aug			b5sus4	sus4	64	(iv 64)

	// classifying seventh chords (dim7 enharmonic to maj6).

	triad \ 7th chord	maj7		min7		dim7

	sus2				maj7sus2	7sus2		sus2add6
	dim					min-maj7b5	min7b5		o7
	min					min-maj7	min7		minadd6
	maj					maj7		7			majadd6
	aug					maj7#5		7#5			#5add6
	sus4				maj7sus4	7sus4		sus4add6
	b5sus2				maj7b5sus2	7b5sus2		42 (II 42)
	#5sus2				maj7#5sus2	7#5sus2		#5sus2add6
	b5sus4				maj7b5sus2	7b5sus4		b5sus4add6
	63					min-maj7#5	min7#5		o7#5				
	64					maj7#5sus4	min7#5sus4	o7#5sus4	
		  

  */


namespace ChordAnalysis
{
inline long Rotate24(long val, long shift)
{
//	ASSERT(shift > -24 && shift < 24);
	int newval = 0;
	for(int i = 0; i < 24; i++)
	{
		long onMask = 1 << i;
		int newpos = (i + shift) % 24;
		if(newpos < 0) newpos = newpos + 24;
		long newOnMask = 1 << newpos;
		if(val & onMask)
		{
			newval |= newOnMask;
		}
	}
	return newval;
}

long NthOne(long scale, long ordinal)
{
	if(ordinal < 0)
		return -1;
	long count = 0;
	long pos = 0;
	for(pos; pos < sizeof(scale)*8 && count < ordinal; pos++)
	{
		if(scale & 0x1)
		{
			count++;
		}
		scale = scale / 2;
	}
	if(count == ordinal)
		return pos - 1;
	else
		return -1;
}

long Third(long scale)
{
	return NthOne(scale, 3);
}

long Fifth(long scale)
{
	return NthOne(scale, 5);
}

long Seventh(long scale)
{
	return NthOne(scale, 7);
}

long ClassifyThird(long interval)
{
	switch(interval)
	{
	case 2:	// dim 3rd
		return 0;
	case 3:	// min 3rd
		return 1;
	case 4:	// maj 3rd
		return 2;
	case 5:	//. aug 3rd
		return 3;
	default:
		return -1;
	}
}

long ClassifyFifth(long interval)
{
	switch(interval)
	{
	case 6:	// dim 5th
		return 0;
	case 7:	// per 5th
		return 1;
	case 8:	// aug 5th
		return 2;
	default:
		return -1;
	}
}

long ClassifySeventh(long interval)
{
	switch(interval)
	{
	case 9:
		return 0;
	case 10:
		return 1;
	case 11:
		return 2;
	default:
		return -1;
	}
}

long Triads[4][3] = 
{
	// dim5, per5, aug5
	{0, 1, 2},	//	dim	3
	{3, 4, 5},	//  min 3
	{6, 7, 8},	//  maj 3
	{9, 10,11}	// aug 3
};

char* TriadNames[12] =
{
	"b5sus2",
	"sus2",
	"#5sus2",
	"dim",
	"min",
	"63",
	"b5",
	"maj",
	"aug",
	"b5sus4",
	"sus4",
	"64"
};


long ClassifyTriad(long third, long fifth)
{
	if(third < 0 || third > 3 || fifth < 0 || fifth > 2)
		return -1;
	else
		return Triads[third][fifth];
}

long SeventhChords[12][3] =
{
	// dim7th, min7th, maj7th
	{0, 1, 2},		// b5sus2
	{3, 4, 5},		// sus2
	{6, 7, 8},		// #5sus2
	{9, 10, 11},	// dim
	{12, 13, 14},	// min
	{15, 16, 17},	// 63
	{18, 19, 20},	// b5
	{21, 22, 23},	// maj
	{24, 25, 26},	// aug
	{27, 28, 29},	// b5sus4
	{30, 31, 32},	// sus4
	{33, 34, 35},	// 64
};

char*	SeventhChordNames[36] =
{
	"42",			"7b5sus2",		"maj7b5sus2",
	"sus2add6",		"7sus2",		"maj7sus2",
	"#5sus2add6",	"7#5sus2",		"maj7#5sus2",
	"o7",			"min7b5",		"min-maj7b5",
	"min-add6",		"min7",			"min-maj7",
	"o7#5",			"min7#5",		"min-maj7#5",
	"b5add6",		"7b5",			"maj7b5",
	"maj-add6",		"7",			"maj7",
	"#5add6",		"7#5",			"maj7#5",
	"b5sus4add6",	"7b5sus4",		"maj7b4sus4",
	"sus4add6",		"7sus4",		"maj7sus4",
	"o7#5sus4",		"min7#5sus4",	"maj7#5sus4"
};

long ClassifySeventhChord(long triad, long seventh)
{
	if(triad < 0 || triad > 11 || seventh < 0 || seventh > 2)
		return -1;
	else
		return SeventhChords[triad][seventh];
}


char* ClassifyChord(long root, long scale)
{
	scale = Rotate24(scale, -root);
	long third, fifth, seventh, triad, seventhchord;
	third = ClassifyThird(Third(scale));
	fifth = ClassifyFifth(Fifth(scale));
	seventh = ClassifySeventh(Seventh(scale));
	triad = ClassifyTriad(third, fifth);
	seventhchord = ClassifySeventhChord(triad, seventh);
	if(seventhchord > 0)
		return SeventhChordNames[seventhchord];
	else if(triad > 0)
		return TriadNames[triad];
	else
		return 0;
}

char* notes[] = { "C", "C#/Db", "D", "D#/Eb", "E", "F", "F#/Gb", "G", "G#/Ab", "A", "A#/Bb", "B" };

long ChordPattern(long root, long scale)
{
	scale = Rotate24(scale, -root);
	long third, fifth, seventh;
	third = Third(scale);
	fifth = Fifth(scale);
	seventh = Seventh(scale);
	long pattern = 1 + (1 << third) + (1 << fifth) + (1 << seventh);
//	pattern = pattern + (pattern << 12);
	pattern = pattern & 0x00ffffff;
	return pattern;
}

long TriadPattern(long root, long scale)
{
	scale = Rotate24(scale, -root);
	long third, fifth;
	third = Third(scale);
	fifth = Fifth(scale);
	long pattern = 1 + (1 << third) + (1 << fifth);
	pattern = pattern & 0x00ffffff;
	return pattern;
}

}




ChordBuilder::ChordBuilder(long scale, int type, int transpose)
{
	UNREFERENCED_PARAMETER(transpose);

	using namespace	ChordAnalysis;
	long mask = 1;
	long idx;
	if(type == Triad)
	{
		for(idx = 0; idx < 12 ; idx++)
		{
			if( scale & mask)
			{
				long tscale = Rotate24(scale, -idx);
				long third, fifth, triad;
				third = ClassifyThird(Third(tscale));
				fifth = ClassifyFifth(Fifth(tscale));
				triad = ClassifyTriad(third, fifth);
				if(triad > 0)
				{
					strcpy(m_chordname[idx], TriadNames[triad]);
					m_chordpattern[idx] = TriadPattern(idx, scale);
					strcpy(m_chordname[idx+12], m_chordname[idx]);
					m_chordpattern[idx+12] = m_chordpattern[idx];
				}
				else
				{
					strcpy(m_chordname[idx], "maj");
					strcpy(m_chordname[idx+12], "maj");
					m_chordpattern[idx] = m_chordpattern[idx+12] = 0x91;
				}
			}
			else
			{
				m_chordpattern[idx] = m_chordpattern[idx+12] = 0x91;
					strcpy(m_chordname[idx], "maj");
					strcpy(m_chordname[idx+12], "maj");
			}
			mask *= 2;
		}
	}
	else
	{
		for(idx = 0; idx < 12 ; idx++)
		{
			if( scale & mask)
			{
				char* pChordName = ClassifyChord(idx, scale);
				if(pChordName)
				{
					strcpy(m_chordname[idx], ClassifyChord(idx, scale));
					m_chordpattern[idx] = ChordAnalysis::ChordPattern(idx, scale);
					strcpy(m_chordname[idx+12], m_chordname[idx]);
					m_chordpattern[idx+12] = m_chordpattern[idx];
				}
				else
				{
					strcpy(m_chordname[idx], "maj");
					strcpy(m_chordname[idx+12], "maj");
					m_chordpattern[idx] = m_chordpattern[idx+12] = 0x91;
				}
			}
			else
			{
				m_chordpattern[idx] = m_chordpattern[idx+12] = 0x91;
				strcpy(m_chordname[idx], "maj");
				strcpy(m_chordname[idx+12], "maj");
			}
			mask *= 2;
		}
	}
}

ChordBuilder::ChordBuilder(long pattern, const char* name)
{
	for(int idx = 0; idx < 24; idx++)
	{
		strcpy(m_chordname[idx], name);
		m_chordpattern[idx] = pattern;
	}
}



