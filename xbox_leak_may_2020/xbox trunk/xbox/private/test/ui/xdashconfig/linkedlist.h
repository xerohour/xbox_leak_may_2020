/*****************************************************
*** linkedlist.h
***
*** Header file for our linked list class.  This class
*** will allow the user to create a linked list using
*** any type of item.
***
*** by James N. Helm
*** October 31st, 2000
*** 
*****************************************************/

#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#define BREAK_INTO_DEBUGGER _asm { int 3 }

template <class T> class CLinkedList
{
public:
    // Constructors and Destructors
    CLinkedList( void );
    ~CLinkedList( void );

    // Public Properties

    // Public Methods
    HRESULT AddNode( const T& item );                               // Add a node to our linked list
    HRESULT DelNode( unsigned int uiIndex );                        // Delete a node from our list by using an index
    HRESULT EmptyList( void );                                      // Empty all of the items out of our list
    unsigned int GetNumItems( void ) const;                         // Get the number of items in our list
    T& GetNode( unsigned int uiIndex ) const;                       // Get a data item from our list by using an index
    T& operator[]( unsigned int uiIndex ) const;                    // Get a data item from our list by using an index

private:
    // Node of our linked list
    struct _LISTNODE
    {
        T dataItem;
        struct _LISTNODE* pNext;
    };

    // Private Properties
    unsigned int m_uicNumItems;         // Number of items in the list
    struct _LISTNODE* m_pHead;

    // Private Methods

};

// Default Constructor.  Properties are initialized here
template< class T > CLinkedList< T >::CLinkedList( void )
{
    m_uicNumItems = 0;
    m_pHead = NULL;
}

// Destructor.  Memory cleanup is performed here
template< class T > CLinkedList< T >::~CLinkedList( void )
{
    // Empty all items from our list
    EmptyList();
}

// Add a node to our list
template< class T > HRESULT CLinkedList< T >::AddNode( const T& item )
{
    // Item to be added to our list
    struct _LISTNODE* pNewItem = new struct _LISTNODE;

    if( !pNewItem )
    {
        DebugPrint( "*** ERROR: CLinkedList():AddNode(): Could not allocate memory!!\n" );
        return E_OUTOFMEMORY;
    }

    pNewItem->dataItem = item;
    pNewItem->pNext = NULL;

    // Check to see if we have a head node.  If not, add the item there
    if( !m_pHead )
    {
        m_pHead = pNewItem;
    }
    else // Add the node to the end of our list
    {
        struct _LISTNODE* pSearchNode = m_pHead;
        
        // Traverse to the end of our list
        while( pSearchNode->pNext )
        {
            pSearchNode = pSearchNode->pNext;
        }

        pSearchNode->pNext = pNewItem;
    }

    // Increment the number of items in our list
    ++m_uicNumItems;

    return S_OK;
}

// Delete a node from our list
template< class T > HRESULT CLinkedList< T >::DelNode( unsigned int uiIndex )
{
    // Make sure our index is valid.  If not, break in to the debugger
    if( ( uiIndex < 0 ) || ( uiIndex >= GetNumItems() ) )
    {
        return E_INVALIDARG;
    };

    struct _LISTNODE* pPrevNode = m_pHead;
    struct _LISTNODE* pSearchNode = m_pHead;

    // If we have a valid index, delete the referenced node
    for( unsigned int x = 0; x < uiIndex; ++x )
    {
        pPrevNode = pSearchNode;
        pSearchNode = pSearchNode->pNext;
    }

    // Deal with the head of the list
    if( pSearchNode == m_pHead )
    {
        m_pHead = m_pHead->pNext;
    }
    else // Not the head node
    {
        // Set our list to skip over the node we'll delete
        pPrevNode->pNext = pSearchNode->pNext;
    }

    // Delete the node
    delete pSearchNode;
    pSearchNode = NULL;

    // Decrement our counter
    --m_uicNumItems;

    return S_OK;
}

// Empty all the items from our list
template< class T > HRESULT CLinkedList< T >::EmptyList( void )
{
    // If there is at least one node in our list, let's clean it up
    if( m_pHead )
    {
        struct _LISTNODE* pSearchNode = m_pHead;
        while( pSearchNode->pNext )
        {
            m_pHead = pSearchNode->pNext;
            delete pSearchNode;
            pSearchNode = m_pHead;
        }

        if( m_pHead )
        {
            delete m_pHead;
            m_pHead = NULL;
            pSearchNode = NULL;
        }
    }

    // Set our item count to zero
    m_uicNumItems = 0;

    return S_OK;
}

// Get a data item from our list by using an index value
template< class T > T& CLinkedList< T >::GetNode( unsigned int uiIndex ) const
{
    // Make sure our index is valid.  If not, break in to the debugger
    if( ( uiIndex < 0 ) || ( uiIndex >= GetNumItems() ) )
    {
        DebugPrint( "CLinkedList GetNode(): index '%d' out of range! MAX - '%d'\n", uiIndex, GetNumItems() );
        BREAK_INTO_DEBUGGER;
    };

    struct _LISTNODE* pSearchNode = m_pHead;

    // If we have a valid index, return a reference to the node
    for( unsigned int x = 0; x < uiIndex; ++x )
    {
        pSearchNode = pSearchNode->pNext;
    }

    return pSearchNode->dataItem;
}

// Get a data item from our list by using an index value
template< class T > T& CLinkedList< T >::operator[]( unsigned int uiIndex ) const
{
    return GetNode( uiIndex );
}

// Get a the number of items in our list
template< class T > unsigned int CLinkedList< T >::GetNumItems( void ) const
{
    return m_uicNumItems;
}

#endif // _LINKEDLIST_H_