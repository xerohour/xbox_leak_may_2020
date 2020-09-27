#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// 03-Oct-94 marklam Removed or modified all the old font dialog and font-caching code

void CFontInfo::InitializeFontInfo(const LOGFONT &lf)
{
	// Fill in the LOGFONT member
	m_lf = lf;

	// Fill in the font Handle
	HDC hDC;
	VERIFY (hDC = ::CreateDC("DISPLAY",NULL,NULL,NULL));
	m_hFont = ::CreateFontIndirect(&lf);

	// Fill in the text metric information
	HFONT hOldFont;
	VERIFY (hOldFont = (HFONT)::SelectObject(hDC, m_hFont));
	VERIFY (::GetTextMetrics(hDC, &m_tm));
	m_fFixedPitchFont = !theApp.m_fOEM_MBCS &&
						!(m_tm.tmPitchAndFamily & TMPF_FIXED_PITCH) &&
						(m_tm.tmAveCharWidth == m_tm.tmMaxCharWidth);

	// Get the Single-byte ASCII widths
	VERIFY (::GetCharWidth(hDC, 0, NASCII_WIDTHS - 1, m_rgcxSBWidths));

	// Clean up GDI resources
	::SelectObject(hDC, hOldFont);
	VERIFY(::DeleteDC(hDC));

	// Mark the lead bytes in the table, and resize others if necessary
	for (unsigned int nChar = 0; nChar < NASCII_WIDTHS; nChar++)
	{
		if ( (theApp.m_fOEM_MBCS) && (_ismbblead(nChar)) )
		{
			// Disable all lead bytes
			m_rgcxSBWidths[nChar] = -1;
		}
		else if (m_tm.tmOverhang)
		{
			// Not a lead byte, but needs resizing
			m_rgcxSBWidths[nChar] -= m_tm.tmOverhang;
		}
	}

	// Empty the trail-byte table pointers
	for (unsigned int nLead = 0; nLead < NLEAD_BYTES; nLead++)
		m_rgrgcxTrailTables[nLead] = NULL;		

	// Mark a tab with an (arbitrary?) length
	m_rgcxSBWidths['\t'] = m_rgcxSBWidths[' '];
}

void CFontInfo::DestroyFontInfo(void)
{
	// Release the memory assigned to the character width tables
	for (UINT nLeadByte = 0; nLeadByte < NLEAD_BYTES; nLeadByte++)
	{
		if (m_rgrgcxTrailTables[nLeadByte] != NULL)
		{
			delete[] m_rgrgcxTrailTables[nLeadByte];
		}
	}

	if (m_hFont != NULL)
		VERIFY(DeleteObject(m_hFont));
}

#define IS_MBCS_FONT(cs) (((cs) == SHIFTJIS_CHARSET)) // These others have to be looked-up! || ((cs) == CHINESEBIG5_CHARSET) || ((cs) == HANGEUL_CHARSET))

int *CFontInfo::rgiLoadTrailWidths(const unsigned char chLead)
{
	int *rgiTrailWidths;

	VERIFY(rgiTrailWidths = new int[NTRAIL_WIDTHS]);		// Out of memory?
	if (rgiTrailWidths == NULL)
		return NULL;

	HDC hDC;
	HFONT hOldFont;
	VERIFY (hDC = ::CreateDC("DISPLAY",NULL,NULL,NULL));
	VERIFY (hOldFont = (HFONT)::SelectObject(hDC, m_hFont));

	if (!::GetCharWidth(hDC, FIRST_CHAR(chLead), LAST_CHAR(chLead), rgiTrailWidths))
	{
		ASSERT (FALSE);										// Couldn't get widths
		delete[] rgiTrailWidths;
		rgiTrailWidths = NULL;
	}
	else
	{
		// Add in tmOverhang
		if (m_tm.tmOverhang != 0)
		{
			for (unsigned char nChar = 0; nChar < NTRAIL_WIDTHS; nChar++)
			{
				rgiTrailWidths[nChar] -= m_tm.tmOverhang;
			}
		}
	}

	::SelectObject(hDC, hOldFont);
	VERIFY(DeleteDC(hDC));

	return rgiTrailWidths;
}
