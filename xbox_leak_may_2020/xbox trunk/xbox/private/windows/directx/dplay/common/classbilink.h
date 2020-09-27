/*==========================================================================
 *
 *  Copyright (C) 1998-1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       ClassBilink.h
 *  Content:	Class-based bilink
 *
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	09/17/99	jtk		Derived from bilink.c
 *	08/15/00	masonb		Changed assert to DNASSERT and added DNASSERT(this)
 *
 ***************************************************************************/

#ifndef __CLASS_BILINK_H__
#define __CLASS_BILINK_H__

#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_COMMON

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
// Function Prototypes
//**********************************************************************

//**********************************************************************
// Class definitions
//**********************************************************************

class	CBilink
{
	STDNEWDELETE

	public:
		CBilink(){};
		~CBilink(){};

		#undef DPF_MODNAME
		#define DPF_MODNAME "CBilink::Initialize"
		void	Initialize( void )
		{
			DNASSERT( this );

			m_pNext = this;
			m_pPrev = this;
		}

		#undef DPF_MODNAME
		#define DPF_MODNAME "CBilink::GetNext"
		CBilink	*GetNext( void ) const 
		{ 
			DNASSERT( this );

			return m_pNext; 
		}

		#undef DPF_MODNAME
		#define DPF_MODNAME "CBilink::GetPrev"
		CBilink *GetPrev( void ) const 
		{ 
			DNASSERT( this );

			return m_pPrev; 
		}

		#undef DPF_MODNAME
		#define DPF_MODNAME "CBilink::IsEmpty"
		BOOL	IsEmpty( void ) const
		{
			DNASSERT( this );
			DNASSERT( m_pNext != NULL );

			if ( ( m_pNext == m_pPrev ) &&
				 ( m_pNext == this ) )
			{
				return	TRUE;
			}

			return	FALSE;
		}

		#undef DPF_MODNAME
		#define DPF_MODNAME "CBilink::IsListMember"
		BOOL	IsListMember( const CBilink *const pList ) const
		{
			DNASSERT( this );
			DNASSERT( pList );

			CBilink	*pTemp = pList->GetNext();
			while ( pTemp != pList )
			{
				if ( pTemp == this )
				{
					return	TRUE;
				}
				pTemp = pTemp->GetNext();
			}

			return	FALSE;
		}

		#undef DPF_MODNAME
		#define DPF_MODNAME "CBilink::InsertAfter"
		void	InsertAfter( CBilink* const pList )
		{
			DNASSERT( this );
			DNASSERT( pList->m_pNext );
			DNASSERT( pList->m_pPrev );
			DNASSERT( !IsListMember( pList ) );

			m_pNext = pList->m_pNext;
			m_pPrev = pList;
			pList->m_pNext->m_pPrev = this;
			pList->m_pNext = this;
		}

		#undef DPF_MODNAME
		#define DPF_MODNAME "CBilink::InsertBefore"
		void	InsertBefore( CBilink* const pList )
		{
			DNASSERT( this );
			DNASSERT( pList->m_pNext );
			DNASSERT( pList->m_pPrev );
			DNASSERT( !IsListMember( pList ) );

			m_pNext = pList;
			m_pPrev = pList->m_pPrev;
			pList->m_pPrev->m_pNext = this;
			pList->m_pPrev = this;
		}

		#undef DPF_MODNAME
		#define DPF_MODNAME "CBilink::RemoveFromList"
		void	RemoveFromList( void )
		{
			DNASSERT( this );
			DNASSERT( m_pNext );
			DNASSERT( m_pPrev );
			DNASSERT( m_pNext->m_pPrev == this );
			DNASSERT( m_pPrev->m_pNext == this );

			m_pNext->m_pPrev = m_pPrev;
			m_pPrev->m_pNext = m_pNext;
			Initialize();
		}

	protected:

	private:
		CBilink	*m_pNext;
		CBilink	*m_pPrev;
};

#undef DPF_MODNAME
#undef DPF_SUBCOMP

#endif	// __CLASS_BILINK_H__
