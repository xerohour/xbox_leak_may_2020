#include "stdafx.h"
#include "PropSwitchPoint.h"


CPropSwitchPoint::CPropSwitchPoint()
{
	Clear();
}

CPropSwitchPoint::CPropSwitchPoint( const CDirectMusicStyleMarker* pDMMarker, DWORD dwVariations )
{
	Clear();
	GetValuesFromDMMarker( pDMMarker, dwVariations );
}

DWORD CPropSwitchPoint::ApplyValuesToDMMarker( CDirectMusicStyleMarker* pDMMarker, DWORD dwVariations ) const
{
	ASSERT( pDMMarker != NULL );

	if( pDMMarker == NULL )
	{
		return 0;
	}

	DWORD dwChanged = 0;

	// Set Variation
	if( m_dwChanged & CHGD_SELVARIATION )
	{
		// Check if any enter variations are set.  If so, check if all of 
		// m_dwVariation are set
		if( pDMMarker->m_dwEnterVariation
		&&	(pDMMarker->m_dwEnterVariation & m_dwVariation) != m_dwVariation )
		{
			pDMMarker->m_dwEnterVariation |= m_dwVariation;
			dwChanged |= CHGD_ENTER;
		}
		// Check if any enter chord variations are set.  If so, check if all of 
		// m_dwVariation are set
		if( pDMMarker->m_dwEnterChordVariation
		&&	(pDMMarker->m_dwEnterChordVariation & m_dwVariation) != m_dwVariation )
		{
			pDMMarker->m_dwEnterChordVariation |= m_dwVariation;
			dwChanged |= CHGD_ENTERCHORD;
		}
		// Check if any exit variations are set.  If so, check if all of 
		// m_dwVariation are set
		if( pDMMarker->m_dwExitVariation
		&&	(pDMMarker->m_dwExitVariation & m_dwVariation) != m_dwVariation )
		{
			pDMMarker->m_dwExitVariation |= m_dwVariation;
			dwChanged |= CHGD_EXIT;
		}
		// Check if any exit chord variations are set.  If so, check if all of 
		// m_dwVariation are set
		if( pDMMarker->m_dwExitChordVariation
		&&	(pDMMarker->m_dwExitChordVariation & m_dwVariation) != m_dwVariation )
		{
			pDMMarker->m_dwExitChordVariation |= m_dwVariation;
			dwChanged |= CHGD_EXITCHORD;
		}
	}

	// Clear Variation
	if( m_dwChanged & CHGD_UNSELVARIATION )
	{
		// Check if any enter variations in m_dwVariation are set
		if( pDMMarker->m_dwEnterVariation & m_dwVariation )
		{
			DWORD dwNewVariation = pDMMarker->m_dwEnterVariation & ~m_dwVariation;
			if( dwNewVariation )
			{
				pDMMarker->m_dwEnterVariation = dwNewVariation;
			}
			dwChanged |= CHGD_ENTER;
		}
		// Check if any enter chord variations in m_dwVariation are set
		if( pDMMarker->m_dwEnterChordVariation & m_dwVariation )
		{
			DWORD dwNewVariation = pDMMarker->m_dwEnterChordVariation & ~m_dwVariation;
			if( dwNewVariation )
			{
				pDMMarker->m_dwEnterChordVariation = dwNewVariation;
			}
			dwChanged |= CHGD_ENTERCHORD;
		}
		// Check if any exit variations in m_dwVariation are set
		if( pDMMarker->m_dwExitVariation & m_dwVariation )
		{
			DWORD dwNewVariation = pDMMarker->m_dwExitVariation & ~m_dwVariation;
			if( dwNewVariation )
			{
				pDMMarker->m_dwExitVariation = dwNewVariation;
			}
			dwChanged |= CHGD_EXIT;
		}
		// Check if any exit chord variations in m_dwVariation are set
		if( pDMMarker->m_dwExitChordVariation & m_dwVariation )
		{
			DWORD dwNewVariation = pDMMarker->m_dwExitChordVariation & ~m_dwVariation;
			if( dwNewVariation )
			{
				pDMMarker->m_dwExitChordVariation = dwNewVariation;
			}
			dwChanged |= CHGD_EXITCHORD;
		}
	}

	if( m_dwChanged & CHGD_ENTER )
	{
		// Check if the enter flag is set
		if( m_wEnterMarkerFlags & DMUS_MARKERF_START )
		{
			// Yes - check if the enter variations are not set
			if( (pDMMarker->m_dwEnterVariation & dwVariations) != dwVariations )
			{
				pDMMarker->m_dwEnterVariation |= dwVariations;
				dwChanged |= CHGD_ENTER;
			}
		}
		else
		{
			// No - check if the enter variations are set
			if( pDMMarker->m_dwEnterVariation & dwVariations )
			{
				pDMMarker->m_dwEnterVariation &= ~dwVariations;
				dwChanged |= CHGD_ENTER;
			}
			// Check if the enter chord variations are set
			if( pDMMarker->m_dwEnterChordVariation & dwVariations )
			{
				pDMMarker->m_dwEnterChordVariation &= ~dwVariations;
				dwChanged |= CHGD_ENTERCHORD;
			}
		}
	}
	if( m_dwChanged & CHGD_ENTERCHORD )
	{
		// Check if the enter chord flag is set
		if( m_wEnterMarkerFlags & DMUS_MARKERF_CHORD_ALIGN )
		{
			// Yes - check if the enter variations are not set
			if( (pDMMarker->m_dwEnterChordVariation & dwVariations) != dwVariations )
			{
				pDMMarker->m_dwEnterChordVariation |= dwVariations;
				dwChanged |= CHGD_ENTERCHORD;
			}
		}
		else
		{
			// No - check if the enter chord variations are set
			if( pDMMarker->m_dwEnterChordVariation & dwVariations )
			{
				pDMMarker->m_dwEnterChordVariation &= ~dwVariations;
				dwChanged |= CHGD_ENTERCHORD;
			}
		}
	}
	if( m_dwChanged & CHGD_EXIT )
	{
		// Check if the exit flag is set
		if( m_wExitMarkerFlags & DMUS_MARKERF_STOP )
		{
			// Yes - check if the exit variations are not set
			if( (pDMMarker->m_dwExitVariation & dwVariations) != dwVariations )
			{
				pDMMarker->m_dwExitVariation |= dwVariations;
				dwChanged |= CHGD_EXIT;
			}
		}
		else
		{
			// No - check if the exit variations are set
			if( pDMMarker->m_dwExitVariation & dwVariations )
			{
				pDMMarker->m_dwExitVariation &= ~dwVariations;
				dwChanged |= CHGD_EXIT;
			}
			// Check if the exit chord variations are set
			if( pDMMarker->m_dwExitChordVariation & dwVariations )
			{
				pDMMarker->m_dwExitChordVariation &= ~dwVariations;
				dwChanged |= CHGD_EXITCHORD;
			}
		}
	}
	if( m_dwChanged & CHGD_EXITCHORD )
	{
		// Check if the exit chord flag is set
		if( m_wExitMarkerFlags & DMUS_MARKERF_CHORD_ALIGN )
		{
			// Yes - check if the exit variations are not set
			if( (pDMMarker->m_dwExitChordVariation & dwVariations) != dwVariations )
			{
				pDMMarker->m_dwExitChordVariation |= dwVariations;
				dwChanged |= CHGD_EXITCHORD;
			}
		}
		else
		{
			// No - check if the exit chord variations are set
			if( pDMMarker->m_dwExitChordVariation & dwVariations )
			{
				pDMMarker->m_dwExitChordVariation &= ~dwVariations;
				dwChanged |= CHGD_EXITCHORD;
			}
		}
	}
	return dwChanged;
}

void CPropSwitchPoint::GetValuesFromDMMarker( const CDirectMusicStyleMarker* pDMMarker, DWORD dwVariations )
{
	// Set the variation flags

	// Check if any of dwVariations have an 'enter' marker
	if( pDMMarker->m_dwEnterVariation & dwVariations )
	{
		// Yes - set m_dwVariation
		m_dwVariation = pDMMarker->m_dwEnterVariation;

		// Check if any of dwVariations have an 'enter chord' marker
		if( pDMMarker->m_dwEnterChordVariation & dwVariations )
		{
			// Yes - update m_dwVarUndetermined and m_dwVariation
			m_dwVarUndetermined = m_dwVariation ^ pDMMarker->m_dwEnterChordVariation;
			m_dwVariation &= pDMMarker->m_dwEnterChordVariation;

			// Check if any of dwVariations have an 'exit' marker
			if( pDMMarker->m_dwExitVariation & dwVariations )
			{
				// Yes - update m_dwVarUndetermined and m_dwVariation
				m_dwVarUndetermined = m_dwVariation ^ pDMMarker->m_dwExitVariation;
				m_dwVariation &= pDMMarker->m_dwExitVariation;

				// Check if any of dwVariations have an 'exit chord' marker
				if( pDMMarker->m_dwExitChordVariation & dwVariations )
				{
					// Yes - update m_dwVarUndetermined and m_dwVariation
					m_dwVarUndetermined = m_dwVariation ^ pDMMarker->m_dwExitChordVariation;
					m_dwVariation &= pDMMarker->m_dwExitChordVariation;
				}
			}
		}
		else
		{
			// Check if any of dwVariations have an 'exit' marker
			if( pDMMarker->m_dwExitVariation & dwVariations )
			{
				// Yes - update m_dwVarUndetermined and m_dwVariation
				m_dwVarUndetermined = m_dwVariation ^ pDMMarker->m_dwExitVariation;
				m_dwVariation &= pDMMarker->m_dwExitVariation;

				// Check if any of dwVariations have an 'exit chord' marker
				if( pDMMarker->m_dwExitChordVariation & dwVariations )
				{
					// Yes - update m_dwVarUndetermined and m_dwVariation
					m_dwVarUndetermined = m_dwVariation ^ pDMMarker->m_dwExitChordVariation;
					m_dwVariation &= pDMMarker->m_dwExitChordVariation;
				}
			}
		}
	}
	else
	{
		// Check if any of dwVariations have an 'exit' marker
		if( pDMMarker->m_dwExitVariation & dwVariations )
		{
			// Yes - set m_dwVariation
			m_dwVariation = pDMMarker->m_dwExitVariation;

			// Check if any of dwVariations have an 'exit chord' marker
			if( pDMMarker->m_dwExitChordVariation & dwVariations )
			{
				// Yes - update m_dwVarUndetermined and m_dwVariation
				m_dwVarUndetermined = m_dwVariation ^ pDMMarker->m_dwExitChordVariation;
				m_dwVariation &= pDMMarker->m_dwExitChordVariation;
			}
		}
	}

	// If all variations are undetermined, set UNDT_VARIATIONS
	if( m_dwVarUndetermined == ALL_VARIATIONS )
	{
		m_dwUndetermined |= UNDT_VARIATIONS;
	}

	// Set the enter flags

	// Check if any of dwVariations have an 'enter' marker
	if( pDMMarker->m_dwEnterVariation & dwVariations )
	{
		// Check if all of dwVariations have an 'enter marker'
		if( (pDMMarker->m_dwEnterVariation & dwVariations) == dwVariations )
		{
			// Yes - set m_wEnterMarkerFlags
			m_wEnterMarkerFlags = DMUS_MARKERF_START;
		}
		else
		{
			// No - 'enter' is undetermined
			m_dwUndetermined |= UNDT_ENTER;
		}

		// Check if any of dwVariations have an 'enter chord' marker
		if( pDMMarker->m_dwEnterChordVariation & dwVariations )
		{
			// Check if all of dwVariations have an 'enter chord' marker
			if( (pDMMarker->m_dwEnterChordVariation & dwVariations) == dwVariations )
			{
				// Yes - set m_wEnterMarkerFlags
				m_wEnterMarkerFlags = DMUS_MARKERF_START | DMUS_MARKERF_CHORD_ALIGN;
			}
			else
			{
				// No - 'enter chord' is undetermined
				m_dwUndetermined |= UNDT_ENTERCHORD;
			}
		}
	}

	// Set the exit flags

	// Check if any of dwVariations have an 'exit' marker
	if( pDMMarker->m_dwExitVariation & dwVariations )
	{
		// Check if all of dwVariations have an 'exit marker'
		if( (pDMMarker->m_dwExitVariation & dwVariations) == dwVariations )
		{
			// Yes - set m_wExitMarkerFlags
			m_wExitMarkerFlags = DMUS_MARKERF_STOP;
		}
		else
		{
			// No - 'exit' is undetermined
			m_dwUndetermined |= UNDT_EXIT;
		}

		// Check if any of dwVariations have an 'exit chord' marker
		if( pDMMarker->m_dwExitChordVariation & dwVariations )
		{
			// Check if all of dwVariations have an 'exit chord' marker
			if( (pDMMarker->m_dwExitChordVariation & dwVariations) == dwVariations )
			{
				// Yes - set m_wExitMarkerFlags
				m_wExitMarkerFlags = DMUS_MARKERF_STOP | DMUS_MARKERF_CHORD_ALIGN;
			}
			else
			{
				// No - 'exit chord' is undetermined
				m_dwUndetermined |= UNDT_EXITCHORD;
			}
		}
	}
}

CPropSwitchPoint CPropSwitchPoint::operator+=( const CPropSwitchPoint PropSwitchPoint )
{
	// Set which variations are undetermined
	m_dwVarUndetermined |= m_dwVariation ^ PropSwitchPoint.m_dwVariation;
	m_dwVarUndetermined |= PropSwitchPoint.m_dwVarUndetermined;

	// Update the variation bits
	m_dwVariation &= PropSwitchPoint.m_dwVariation;

	// If all variations are undetermined, set UNDT_VARIATIONS
	if( m_dwVarUndetermined == ALL_VARIATIONS )
	{
		m_dwUndetermined |= UNDT_VARIATIONS;
	}

	// Update our undetermined flags
	m_dwUndetermined |= PropSwitchPoint.m_dwUndetermined;

	// Enter flag
	if( !(m_dwUndetermined & UNDT_ENTER)
	&&	(m_wEnterMarkerFlags & DMUS_MARKERF_START) != (PropSwitchPoint.m_wEnterMarkerFlags & DMUS_MARKERF_START) )
	{
		m_dwUndetermined |= UNDT_ENTER;
	}

	// Enter chord flag
	if( !(m_dwUndetermined & UNDT_ENTERCHORD)
	&&	(m_wEnterMarkerFlags & DMUS_MARKERF_CHORD_ALIGN) != (PropSwitchPoint.m_wEnterMarkerFlags & DMUS_MARKERF_CHORD_ALIGN) )
	{
		m_dwUndetermined |= UNDT_ENTERCHORD;
	}

	// Exit flag
	if( !(m_dwUndetermined & UNDT_EXIT)
	&&	(m_wExitMarkerFlags & DMUS_MARKERF_STOP) != (PropSwitchPoint.m_wExitMarkerFlags & DMUS_MARKERF_STOP) )
	{
		m_dwUndetermined |= UNDT_EXIT;
	}

	// Exit chord flag
	if( !(m_dwUndetermined & UNDT_EXITCHORD)
	&&	(m_wExitMarkerFlags & DMUS_MARKERF_CHORD_ALIGN) != (PropSwitchPoint.m_wExitMarkerFlags & DMUS_MARKERF_CHORD_ALIGN) )
	{
		m_dwUndetermined |= UNDT_EXITCHORD;
	}

	return *this;
}

void CPropSwitchPoint::Copy( CPropSwitchPoint *pPropSwitchPoint )
{
	if( pPropSwitchPoint == NULL )
	{
		Clear();
		return;
	}

	m_dwVariation = pPropSwitchPoint->m_dwVariation;
	m_wEnterMarkerFlags = pPropSwitchPoint->m_wEnterMarkerFlags;
	m_wExitMarkerFlags = pPropSwitchPoint->m_wExitMarkerFlags;

	m_dwUndetermined = pPropSwitchPoint->m_dwUndetermined;
	m_dwVarUndetermined = pPropSwitchPoint->m_dwVarUndetermined;

	m_dwChanged = pPropSwitchPoint->m_dwChanged;
}

void CPropSwitchPoint::Clear()
{
	m_dwVariation = 0;
	m_wEnterMarkerFlags = 0;
	m_wExitMarkerFlags = 0;

	m_dwUndetermined = 0;
	m_dwVarUndetermined = 0;

	m_dwChanged = 0;
}
