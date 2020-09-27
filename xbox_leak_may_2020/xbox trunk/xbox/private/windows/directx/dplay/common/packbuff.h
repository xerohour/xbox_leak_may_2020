/*==========================================================================
 *
 *  Copyright (C) 1998-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       PackBuff.h
 *  Content:	Packed Buffers
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	01/11/00	mjn		Created
 *  06/15/2000  rmt     Added func to add string to packbuffer
 ***************************************************************************/

#ifndef __PACK_BUFF_H__
#define __PACK_BUFF_H__

//**********************************************************************
// Constant definitions
//**********************************************************************

//**********************************************************************
// Macro definitions
//**********************************************************************

//**********************************************************************
// Structure definitions
//**********************************************************************

//**********************************************************************
// Variable definitions
//**********************************************************************

//**********************************************************************
// Function prototypes
//**********************************************************************

//**********************************************************************
// Class prototypes
//**********************************************************************

// class for packed buffer

class CPackedBuffer
{
	STDNEWDELETE

public:
	CPackedBuffer() { };	// Constructor

	~CPackedBuffer() { };	// Destructor

	void	Initialize(	void *const pvBuffer,
						const DWORD dwBufferSize );

	HRESULT AddToFront( const void *const pvBuffer,
						const DWORD dwBufferSize );

	HRESULT AddToBack( const void *const pvBuffer,
						const DWORD dwBufferSize );

	PVOID GetStartAddress( void ) { return m_pStart; };

    HRESULT AddWCHARStringToBack( const wchar_t * const pwszString );					

	PVOID GetHeadAddress( void ) const { return( m_pHead ); }

	PVOID GetTailAddress( void ) const { return( m_pTail ); }

	DWORD GetHeadOffset( void ) const
	{
		return( (DWORD)(m_pHead - m_pStart) );
	}

	DWORD GetTailOffset( void ) const
	{
		return( (DWORD)(m_pTail - m_pStart) );
	}

	DWORD GetSpaceRemaining( void ) const { return( m_dwRemaining ); }

	DWORD GetSizeRequired( void ) const { return( m_dwRequired ); }

private:
	BYTE	*m_pStart;			// Start of the buffer
	BYTE	*m_pHead;			// Pointer to head of free buffer
	BYTE	*m_pTail;			// Pointer to tail of free buffer
	DWORD	m_dwRemaining;		// bytes remaining in buffer
	DWORD	m_dwRequired;		// bytes required so far
	BOOL	m_bBufferTooSmall;	// buffer has run out of space
};



#endif	// __PACK_BUFF_H__
