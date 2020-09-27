#ifndef INVERTVECTOR_H
#define INVERTVECTOR_H

/*

  The purpose of this object is to assist in applying intelligent voice leading. Given a pattern and
  a chord, the user can determine which chord inversions are allowed when transposing portions of a
  pattern that extend beyond an upper or lower (MIDI note) boundary.  For this to work, the pattern
  must be in root position and all voices within one octave.

  This algorithm uses two DWORD bitmasks.  The Chord position bit mask is used to determine where
  the root first, second, and third inversions are.  This mask is used for internal computations.
  The inversion mask is the resulting mask used by DirectMusic to determine which transpositions
  are allowed.  The input to the algorithm is the chord position bit mask, the output

  The chord position bitmask is defined as follows

  Root of chord = position of 1st bit in pattern whose value is 1.  This is always bit 0 of the chord mask.
  Third of chord = position of 2nd bit in pattern whose value is 1
  Fifth of chord = position of 3rd bit in pattern whose value is 1
  Seventh of chord = position of 4th bit in pattern whose value is 1

  Inversions can be characterized by chord functions

  Root position = original chord pattern: from top to bottom: root, 3rd, 5th, optional 7th
  1st Inversion = 3rd is lowest note
  2nd Inversion = 5th is lowest note
  3rd Inversion = 7th is bass note

  Example:

  CM7 = 0x891 = 0100 1001 0001
  Cm7 = 0x489 = 0100 1000 1001

  The default inversion mask is 0x00000000.  This blocks all inversions, i.e., the pattern is not transposed.
  To block a single inversion, we want to keep the note defining the inversion out of the bass.  For example,
  to block a first inversion chord, we must keep the third from being the lowest tone.  This will be the
  case if we force the root always to be below the third.  Thus when transposing either the root and third must
  be transposed as a pair or must remain in place.

  This can be extended to block multiple inversions.  For example to block the second and third inversions and allow 
  the first,   we must force the third and fifth to stay together (blocking the second inversion) and the fifth and 
  seventh together (blocking the third inversion).  To block all three inversions, we force the root and third to
  stay together (1st inversion), the third and fifth to stay together (2nd inversion) and the fifth and seventh
  to stay together( 3rd inversion).  We thus get a the following schema for blocking inversions:

  Inversion to block	Notes to keep together
  ------------------	----------------------
  1st inversion			root and third
  2nd inversion			third and fifth
  3rd inversion			fifth and seventh

  Now we need to discuss how direct music computes where to invert.  Given an upper and lower boundary (specified as
  MIDI notes in the pattern editor) and a note in the pattern outside one of those boundaries, it maps the boundary
  to the corresponding position in the inversion mask and scans up from the lower boundary or down from the upper
  boundary till it either hits a "1" in the inversion or the offending note.  If the former, inversion is allowed,
  otherwise it is blocked.

  The chord position mask is used to determine where the positions are in the given chord.  Thus to block a certain
  inversion, we determine the notes to keep together and set all the bit positions in between them to 0's.  The
  zero'd bits include the lower member (eg root in the first inversion case), but not the upper member (this is to
  satisfy some differences in the scan directions).  A complete table of masks for blocking inversions in a Cm7
  chord (0x489 = 0100 1000 1001) follows (masks are shown for one octave only.  In practise they are doubled
  at the octave and the upper 8 bits are set to zero

  Inversion to block		Notes to keep together			inversion mask
  ------------------		----------------------			--------------
  1st						Root and third					1111 1111 0000
  2nd						third and fifth					1111 1000 1111
  3rd						fifth and seventh				1100 0111 1111
  
  An inversion is disallowed if the bits in its pattern between its bass note and the next lower chord member are zero
  inclusive at the bottom, exclusive at the top.  Let ChordMember(n) return the bit position of the nth chord member.  Thus
  ChordMember(0) returns the root, ChordMember(1) returns the third, and so on.  Then the nth inversion, n > 0, is disallowed
  if

  Bitpos(m) == 0 for ChordMember(n-1)<= m < ChordMember(n)

  Using the above example,	0100 1001 0001

  Disallow 1st Inversion:	1111 1111 0000
  Disallow 2nd Inversion:  1111 1000 1111
  Disallow 3rd Inversion:  1100 0111 1111

  Inversion Arithmetic:

  Inversions disallows can be combined by anding them:

  Disallow 1st and 3rd Inversion: 1100 0111 0000 == 1111 1111 0000 & 1100 0111 1111

  Inversions can be reallowed by or'ing in the complement of the disallowal mask.  For example we can reallow 3rd inversion in
  1100 0111 0000, by oring in 0011 1000 0000

  Given the bit positions of the chord members, a disallowal mask for the nth inversion can be computed as follows:

	DWORD mask = 0xFFFFFFFF;
	for(int i = ChordMember(n-1); i < ChordMember(n); i++)
	{
		mask &= ~(1 << i);
	}

  Given an inversion vector, and assuming disallowal masks have been computed, an inversion, N,  is disallowed if

	(1 << ChordMember(N-1)+1) & DisallowalMask[N] = 0;

  Where DisallowalMask[N] is the disallowal mask for the Nth inversion.  Note the disallowed condition in the inversion vector
  is weaker than the mask, this is because inversion patterns also apply to scales.


*/

#ifdef TESTIT
#include <stdio.h>
#endif

class InvertVector
{
public:
	enum { MAX_INVERT = 3, ALL = -1 };
private:
	unsigned long	ulInvert;
	unsigned long	ulChord;
	int	ChordMemberBitPos[MAX_INVERT+1];
	unsigned long DisallowedMasks[MAX_INVERT];
	int highestChordMember;
public:
	InvertVector(unsigned long chord = 0x91, unsigned long invert = 0xFFFFFFFF);
	int MaxInversions() { return highestChordMember; }
	unsigned long GetInvertPattern() const
	{
		return ulInvert;
	}
	unsigned long GetChordPattern() const
	{
		return ulChord;
	}
	void SetInversion(unsigned long inversion)
	{
		ulInvert = inversion;
	}
	void SetChord(unsigned long chord);
	void SetAllowedInversion(int inversion, bool allowed);
	bool GetAllowedInversion(int inversion) const
	{
		if(inversion < 0 || inversion >= highestChordMember)
			return false;
		return (ulInvert & (1 << (ChordMemberBitPos[inversion] + 1))) != 0;
	}
#ifdef TESTIT
	const char* Dump()
	{
		static char buf[200];
		sprintf(buf, "chord: %x\t invert: %x\n", ulChord, ulInvert);
		return buf;
	}
#endif
private:
	void FindBitPos();
	void ComputeDisallowedMasks();
};



#endif