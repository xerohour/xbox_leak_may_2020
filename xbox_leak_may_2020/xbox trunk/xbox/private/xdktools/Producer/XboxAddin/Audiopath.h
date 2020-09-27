#if !defined(AFX_AUDIOPATH_H__45DF9671_11A3_4073_9CE0_6F8456E61502__INCLUDED_)
#define AFX_AUDIOPATH_H__45DF9671_11A3_4073_9CE0_6F8456E61502__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Audiopath.h : header file
//

#include "FileItem.h"

/////////////////////////////////////////////////////////////////////////////
// CAudiopath class

class CAudiopath : public CFileItem
{
public:
	CAudiopath( IDMUSProdNode *pDMUSProdNode, DWORD dwType );
	HRESULT CopyToXbox( void );
	HRESULT Release( void );
	HRESULT SetDefault( bool fDefault );
	inline bool IsDefault( void ) const { return m_fDefault; }
	virtual HRESULT RemoveFromXbox( void );

	DWORD			m_dwIndex;
	DWORD			m_dwStandardType;
	DWORD			m_dwStandardPChannels;

protected:
	bool			m_fDefault;
	static DWORD	m_sdwNextIndex;
};


#endif // !defined(AFX_AUDIOPATH_H__45DF9671_11A3_4073_9CE0_6F8456E61502__INCLUDED_)
