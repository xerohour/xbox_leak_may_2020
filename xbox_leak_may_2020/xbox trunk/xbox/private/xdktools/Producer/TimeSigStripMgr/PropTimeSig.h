#ifndef __PROPTIMESIG_H_
#define __PROPTIMESIG_H_

#include <DMUSProd.h>
#include <dmusici.h>

class CTimeSigItem;

class CPropTimeSig 
{
public:
	CPropTimeSig();
	CPropTimeSig( const CTimeSigItem *pTimeSigItem );
	~CPropTimeSig();
	const TCHAR *GetString();

    DWORD				m_dwMeasure;		// What measure this Time Signature falls on
	DWORD				m_dwBits;			// Various bits
	DMUS_TIMESIGNATURE	m_TimeSignature;	// Time Signature
	TCHAR				m_tcsText[20];		// Textual representation of the Time Signature
};

#define UD_DRAGSELECT		0x0001
#define UD_FAKE				0x0004
#define UD_STYLEUPDATE		0x0008

#endif // __PROPTIMESIG_H_
