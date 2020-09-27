#ifndef __SHAREDPATTERN_H__
#define __SHAREDPATTERN_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// SharedPattern.h : header file
//

#define DX8_PART_SIZE 160
#define DX8_PARTREF_SIZE 28

// Pattern Flags
#define PF_FILL     0x0001      // Fill pattern
#define PF_START    0x0002      // May be starting pattern
#define PF_INTRO    0x0002
#define PF_WHOLE    0x0004      // Handles chords on measures
#define PF_HALF     0x0008      // Chords every two beats
#define PF_QUARTER  0x0010      // Chords on beats
#define PF_BREAK    0x0020
#define PF_END      0x0040
#define PF_A        0x0080
#define PF_B        0x0100
#define PF_C        0x0200
#define PF_D        0x0400
#define PF_E        0x0800
#define PF_F        0x1000
#define PF_G        0x2000
#define PF_H        0x10000
#define PF_STOPNOW  0x4000
#define PF_INRIFF   0x8000
#define PF_MOTIF    0x20000     // this pattern is a motif, not a regular pattern
#define PF_BEATS    ( PF_WHOLE | PF_HALF | PF_QUARTER )
#define PF_RIFF     ( PF_INTRO | PF_BREAK | PF_FILL | PF_END )
#define PF_GROOVE   ( PF_A | PF_B | PF_C | PF_D | PF_E | PF_F | PF_G | PF_H )


// Curve flip flags
#define CURVE_FLIPTIME	1
#define CURVE_FLIPVALUE	2


// Chord flags
#define CHORD_INVERT    0x10        // This chord may be inverted       
#define CHORD_FOUR      0x20        // This should be a 4 note chord    
#define CHORD_UPPER     0x40        // Shift upper octave down          
#define CHORD_SIMPLE    0x80        // This is a simple chord           
#define CHORD_COUNT     0xF         // Chord count bits (up to 15)      
#define CHORD_EX_INVERT 0x10000000  // This chord may be inverted       
#define CHORD_EX_FOUR   0x20000000  // This should be a 4 note chord    
#define CHORD_EX_UPPER  0x40000000  // Shift upper octave down          
#define CHORD_EX_SIMPLE 0x80000000  // This is a simple chord           
#define CHORD_EX_COUNT  0x0F000000  // Chord count bits (up to 15)      
#define CHORD_EX_BITS   0xFF000000  // Flag bits region of chord.
#define CHORD_PATTERN   0x00FFFFFF  // Pattern region of chord.


// Embellishment flags
#define EMB_NORMAL	0
#define EMB_FILL	1
#define EMB_BREAK	2
#define EMB_INTRO	4
#define EMB_END		8
#define EMB_MOTIF	16
#define EMB_ALL		0xFFFF


// Conversion macros
#define TS_GRID_TO_CLOCKS( grid, TimeSig ) ( ((grid) / (TimeSig).m_wGridsPerBeat) * (DM_PPQNx4 / (TimeSig).m_bBeat) + ((grid) % (TimeSig).m_wGridsPerBeat) * ((DM_PPQNx4 / (TimeSig).m_bBeat) / (TimeSig).m_wGridsPerBeat) )
#define TS_CLOCKS_TO_GRID( clocks, TimeSig ) ( ((clocks) / (DM_PPQNx4 / (TimeSig).m_bBeat)) * (TimeSig).m_wGridsPerBeat + ((clocks) % (DM_PPQNx4 / (TimeSig).m_bBeat)) / ((DM_PPQNx4 / (TimeSig).m_bBeat) / (TimeSig).m_wGridsPerBeat) )

#define GRID_TO_CLOCKS( grid, pDMPart ) ( ((grid) / (pDMPart)->m_TimeSignature.m_wGridsPerBeat) * (pDMPart)->m_mtClocksPerBeat + ((grid) % (pDMPart)->m_TimeSignature.m_wGridsPerBeat) * (pDMPart)->m_mtClocksPerGrid )
#define CLOCKS_TO_GRID( clocks, pDMPart ) ( ((clocks) / (pDMPart)->m_mtClocksPerBeat) * (pDMPart)->m_TimeSignature.m_wGridsPerBeat + ((clocks) % (pDMPart)->m_mtClocksPerBeat) / (pDMPart)->m_mtClocksPerGrid )


// Number of variations
#define NBR_VARIATIONS	32

#define DM_FOURCC_VARCHOICES_FORM    mmioFOURCC('v','a','r','f')
#define DM_FOURCC_VARCHOICES_CHUNK   mmioFOURCC('v','a','r','h')

// Format used for Copy/Paste
typedef struct ioVarChoices
{
    DWORD   m_dwVariationChoices[NBR_VARIATIONS];	// MAOW VarChoices bit field
	DWORD   m_dwSelectedFlagBtns[NBR_VARIATIONS];	// selected function buttons
	BYTE    m_bSelectedRowBtns[NBR_VARIATIONS];		// selected function buttons
} ioVarChoices;


// Various types of modes (rows) ////////////////////////////
#define IMA_ROWTYPE		0
#define DM_ROWTYPE		1
#define NBR_ROWTYPES	2
#define IMA_NBR_COLUMNS		15
#define DM_NBR_COLUMNS		30


// Variation Choices Flags
#define IMA_VF_FLAG_BITS	0x00007FFF	 // Flag bits used for IMA mode 
#define DM_VF_FLAG_BITS		0x5FFFFFFF	 // Flag bits used for all other modes 
#define DM_VF_MODE_BITS		0xA0000000	 // 2 bits reserved for modes
#define DM_VF_MODE_IMA		0x00000000	 // IMA mode 
#define DM_VF_MODE_DMUSIC	0x20000000	 // DirectMusic mode 

#define DM_VF_MAJOR			0x0000007F   // Seven positions in the scale - major chords.    
#define DM_VF_MINOR			0x00003F80   // Seven positions in the scale - minor chords.    
#define DM_VF_OTHER			0x001FC000   // Seven positions in the scale - other chords.    
#define DM_VF_ROOT_SCALE	0x00200000   // Handles chord roots in the scale.         
#define DM_VF_ROOT_FLAT		0x00400000   // Handles flat chord roots (based on scale notes).         
#define DM_VF_ROOT_SHARP	0x00800000   // Handles sharp chord roots (based on scale notes).         
#define DM_VF_TYPE_TRIAD	0x01000000   // Handles simple chords - triads.  
#define DM_VF_TYPE_6AND7	0x02000000   // Handles simple chords - 6 and 7.  
#define DM_VF_TYPE_COMPLEX  0x04000000   // Handles complex chords.  
#define DM_VF_DEST_TO1      0x08000000   // Handles transitions to 1 chord.  
#define DM_VF_DEST_TO5      0x10000000   // Handles transitions to 5 chord. 
#define DM_VF_DEST_TOOTHER  0x40000000   // Handles transitions to other chord. 


#define SUBCHORD_BASS		0
#define SUBCHORD_UPPER_1	1
#define SUBCHORD_UPPER_2	2
#define SUBCHORD_UPPER_3	3


interface IDMUSProdFramework;
IStream *CreateStreamForVarChoices( IDMUSProdFramework *pIDMUSProdFramework, DWORD adwVariationChoices[NBR_VARIATIONS] );

#endif // __SHAREDPATTERN_H__
