//////////////////////////////////////////////////////////////////////
// RegionNote.h 
//

#ifndef REGIONOTE_H
#define REGIONOTE_H

// If any new members are add to this class make sure they are initialize 
// CRegionKeyboardCtrl's constructor
class RegionNote
{
	public:
		bool m_bMemberOfRegion;		// Indicates if note is a member of a region
		bool m_bStartOfRegion;		// Indicates if note is the first note of a region
		int m_nFirstNote;			// First note of region that contains note
		int m_nLastNote;			// Last note of region that contains note
		int m_nStartPixel;			// Start pixel for region that contains note	
		int m_nEndPixel;			// End pixel for region that contains note
		int m_nStartOfNextRegion;	// Note which starts next region 
		int m_nEndOfPrevRegion;     // Note which ends previous region
		int m_nIDRegion;			// ID which indicates which region in instrument note belongs to
        bool m_bIsRootNote;         // indicates if this note is a root note
};

#endif
