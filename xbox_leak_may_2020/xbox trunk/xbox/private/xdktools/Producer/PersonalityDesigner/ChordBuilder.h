#ifndef CHORDBUILDER_H
#define CHORDBUILDER_H


class ChordBuilder
{
	long m_chordpattern[24];
	char m_chordname[24][20];
public:
	enum {Triad, Seventh, AllMaj, AllMin, AllMaj7, AllMin7, AllDom7};	// types
	ChordBuilder(long scale, int type, int transpose);	// deduce diatonic chords in scale
	ChordBuilder(long pattern, const char* name);	// build identical chords with given pattern and name
	long ChordPattern(int root)	// chord pattern on ith root
	{
		if(-1 < root && root < 24)
		{
			return m_chordpattern[root];
		}
		else
			return -1;
	}
	const char* ChordName(int root)	// chord name on ith root
	{
		if(-1 < root && root < 24)
		{
			return m_chordname[root];
		}
		else
			return 0;
	}
};


#endif