#ifndef __PROPSWITCHPOINT_H_
#define __PROPSWITCHPOINT_H_

#include "PropNote.h"

class CPropSwitchPoint
{
public:
	CPropSwitchPoint();
	CPropSwitchPoint( const CDirectMusicStyleMarker* pDMMarker, DWORD dwVariations );

	DWORD	ApplyValuesToDMMarker( CDirectMusicStyleMarker* pDMMarker, DWORD dwVariations ) const;
	void	GetValuesFromDMMarker( const CDirectMusicStyleMarker* pDMMarker, DWORD dwVariations );
	CPropSwitchPoint operator+=( const CPropSwitchPoint PropSwitchPoint );
	void	Copy( CPropSwitchPoint *pPropSwitchPoint );
	void	Clear();

	// Event fields
	DWORD		m_dwVariation;		// Variation bits
	
	// Marker fields	
    WORD        m_wEnterMarkerFlags;	/* how the enter marker is used */
    WORD        m_wExitMarkerFlags;		/* how the exit marker is used */
	
	// Used to track which fields are shared by multiple 
	// items and have multiple values, so are undetermined.
	DWORD		m_dwUndetermined;
	DWORD		m_dwVarUndetermined;		// Which variations are undetermined

	// Used to relay which fields have changed
	DWORD		m_dwChanged;
};


// Flags for m_dwUndetermined:

#define UNDT_VARIATIONS		UD_VARIATIONS
#define CHGD_SELVARIATION	CHGD_VARIATIONS
#define CHGD_UNSELVARIATION	(1 << 23)

#define UNDT_ENTER			(1 << 19)
#define UNDT_ENTERCHORD		(1 << 20)
#define UNDT_EXIT			(1 << 21)
#define UNDT_EXITCHORD		(1 << 22)

#define CHGD_ENTER			UNDT_ENTER	
#define CHGD_ENTERCHORD		UNDT_ENTERCHORD		
#define CHGD_EXIT			UNDT_EXIT		
#define CHGD_EXITCHORD		UNDT_EXITCHORD	

#endif // __PROPSWITCHPOINT_H_
