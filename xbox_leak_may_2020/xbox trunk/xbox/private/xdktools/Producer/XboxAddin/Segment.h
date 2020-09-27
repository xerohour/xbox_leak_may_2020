#if !defined(AFX_SEGMENT_H__52EF9671_11A3_4073_9CE0_6F8456E61502__INCLUDED_)
#define AFX_SEGMENT_H__52EF9671_11A3_4073_9CE0_6F8456E61502__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Segment.h : header file
//

#include "FileItem.h"

class CAudiopath;

/////////////////////////////////////////////////////////////////////////////
// CSegment class

class CSegment : public CFileItem
{
public:
	CSegment( IDMUSProdNode *pDMUSProdNode );
	void ButtonClicked( void );
	virtual HRESULT CopyToXbox( void );
	HRESULT Unload( void );
	virtual void OnSegEnd( void );

	bool			m_fPlaying;
	bool			m_fPressed;
	DWORD			m_dwPlayFlags;
	DWORD			m_dwIndex;
	CAudiopath		*m_pAudiopath;
	DWORD			m_dwStandardAudiopath;

protected:
	static DWORD	m_sdwNextIndex;
};


#endif // !defined(AFX_SEGMENT_H__52EF9671_11A3_4073_9CE0_6F8456E61502__INCLUDED_)
