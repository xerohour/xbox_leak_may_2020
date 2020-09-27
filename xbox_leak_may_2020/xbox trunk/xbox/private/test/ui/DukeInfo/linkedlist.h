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
    HRESULT AddNode( const T& item );                                           // Add a node to our linked list
    HRESULT DelNode( unsigned int uiIndex );                                    // Delete a node from our list by using an index
    HRESULT EmptyList( void );                                                  // Empty all of the items out of our list
    HRESULT MoveTo( unsigned int uiIndex );                                     // Move to the specified item in our list via index
    HRESULT MoveNext();                                                         // Move to the next item in the list
    HRESULT MovePrev();                                                         // Move to the prev item in the list
    unsigned int GetNumItems( void ) const;                                     // Get the number of items in our list
    void SetDeleteDataItem( bool bDelItem ) { m_bDeleteDataItem = bDelItem; };  // Set this if the list needs to delete the dataitem
    T& GetCurrentNode() const;                                                  // Get a data item from the current node in our list
    T& GetNode( unsigned int uiIndex );                                         // Get a data item from our list by using an index
    T& operator[]( unsigned int uiIndex );                                      // Get a data item from our list by using an index

private:
    // Node of our linked list
    struct _LISTNODE
    {
        T dataItem;
        struct _LISTNODE* pNext;
        struct _LISTNODE* pPrev;
    };

    // Private Properties
    unsigned int m_uicNumItems;         // Number of items in the list
    struct _LISTNODE* m_pHead;          // Head of our list
    struct _LISTNODE* m_pLast;          // Last Node of our list
    struct _LISTNODE* m_pCurrent;       // Points at the current item in the list
    unsigned int m_uicCurrentItemIndex; // Index of the item that is currently being pointed at
    bool m_bDeleteDataItem;             // This needs to be TRUE if you are passing in a structure / item that was 'new'ed

    // Private Methods

};


// Default Constructor.  Properties are initialized here
template< class T > CLinkedList< T >::CLinkedList( void )
: m_bDeleteDataItem( false),
  m_uicNumItems( 0 ),
  m_uicCurrentItemIndex( 0 ),
  m_pHead( NULL ),
  m_pLast( NULL),
  m_pCurrent( NULL )
{
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
        XDBGERR( APP_TITLE_NAME_A, "*** ERROR: CLinkedList():AddNode(): Could not allocate memory!!" );
        return E_OUTOFMEMORY;
    }

    pNewItem->dataItem = item;
    pNewItem->pNext = NULL;
    pNewItem->pPrev = NULL;

    // Check to see if we have a head node.  If not, add the item there
    if( !m_pHead )
    {
        m_pCurrent = m_pLast = m_pHead = pNewItem;
        m_uicCurrentItemIndex = 1;  // Points at the head node
        
    }
    else // Add the node to the end of our list
    {
        m_pLast->pNext = pNewItem;  // Add the new node to our list
        pNewItem->pPrev = m_pLast;  // Update the new nodes previous pointer
        m_pLast = pNewItem;         // Update our pLast pointer to point at the new last node
    }

    // Increment the number of items in our list
    ++m_uicNumItems;

    return S_OK;
}


// Delete a node from our list
template< class T > HRESULT CLinkedList< T >::DelNode( unsigned int uiIndex )
{
    // Make sure our index is valid.  If not, break in to the debugger
    if( uiIndex >= GetNumItems() )
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
        // Check to see if it's the last node, and update our
        // our pointers
        if( m_pLast == m_pHead )
        {
            m_pCurrent = m_pLast = m_pHead = NULL;
        }
        else if( m_pCurrent = m_pHead ) // Deal with the 'Current' pointer if we have to
        {
            m_pCurrent = m_pHead = m_pHead->pNext;       // If head is the only node, m_pHead->pNext will be NULL
        }
        else // Only deal with the head node
        {
            m_pHead = m_pHead->pNext;       // If head is the only node, m_pHead->pNext will be NULL
        }

        // Check to see if our current point still points at anything, and if not,
        // make sure we set the Item Index to 0
        if( !m_pCurrent )
        {
            m_uicCurrentItemIndex = 0;
        }

        // Check to see if the new head node points at anything, and if so,
        // make sure the pPrev pointer is NULL
        if( m_pHead )
        {
            m_pHead->pPrev = NULL;
        }
    }
    else if( pSearchNode == m_pLast )   // Delete the last node in our list
    {
        // Check to see if our current node is pointing at the last node, and deal with it
        if( m_pCurrent == m_pLast )
        {
            m_pCurrent = m_pLast = pPrevNode;
            --m_uicCurrentItemIndex;            // Decrement our current item index since we were pointing at the last node
        }
        else // Only deal with the last node pointer, and don't worry about the current node pointer
        {
            m_pLast = pPrevNode;
        }
    }
    else // Not the head node or the last node ( middle node! )
    {

        // Check to see if our current node is this node, and if so,
        // move it along as well
        if( pSearchNode = m_pCurrent )
        {
            // Set our list to skip over the node we'll delete, and set our current pointer to the same item
            m_pCurrent = pPrevNode->pNext = pSearchNode->pNext;
        }
        else // Don't worry about the current node pointer, simply skip the node we'll delete
        {
            // Set our list to skip over the node we'll delete
            pPrevNode->pNext = pSearchNode->pNext;
        }
    }

    if( m_bDeleteDataItem )
    {
        delete pSearchNode->dataItem;
        pSearchNode->dataItem = NULL;
    }

    // Delete the node
    delete pSearchNode;
    pSearchNode = NULL;

    // Decrement our number of items counter
    --m_uicNumItems;

    return S_OK;
}


// Empty all the items from our list
template< class T > HRESULT CLinkedList< T >::EmptyList( void )
{
    HRESULT hr = S_OK;

    // Call our "DelNode" function until we are out of items
    while( GetNumItems() > 0 )
    {
        if( FAILED( hr = DelNode( 0 ) ) )
        {
            XDBGERR( APP_TITLE_NAME_A, "CLinkedList::EmptyList():Failed to delete node!!" );
        }
    }

    return hr;
}


// Get a data item from the current node in our list
template< class T > T& CLinkedList< T >::GetCurrentNode() const
{
    if( m_pCurrent )
    {
        return m_pCurrent->dataItem;
    }

    // This will only happen if the current pointer isn't pointing at a node
    // shame on the developer!!
    XDBGWRN( APP_TITLE_NAME_A, "CLinkedList GetCurrentNode():BREAKING IN TO THE DEBUGGER!!" );
    XDBGERR( APP_TITLE_NAME_A, "CLinkedList GetCurrentNode()No node at the current pointer!!" );
    BREAK_INTO_DEBUGGER; // Can't return anything nice here, so break in to the debugger
    
    return m_pCurrent->dataItem; // This line should never be executed, as m_pCurrent isn't valid!!
}


// Get a data item from our list by using an index value
template< class T > T& CLinkedList< T >::GetNode( unsigned int uiIndex )
{
    // Move to the requested Node in the list
    // MoveTo will handle an invalid index case
    HRESULT hr = MoveTo( uiIndex );
    if( FAILED( hr ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CLinkedList GetNode():BREAKING IN TO THE DEBUGGER!!" );
        XDBGERR( APP_TITLE_NAME_A, "CLinkedList GetNode():MoveTo failed!" );
        BREAK_INTO_DEBUGGER;    // Can't return anything nice here, so break in to the debugger
    }

    // Return the requested node to the user
    return GetCurrentNode();
}


// Move to the specified item in our list via index
template< class T > HRESULT CLinkedList< T >::MoveTo( unsigned int uiIndex )
{
    // Make sure our index is valid.  If not, return an error
    if( uiIndex >= GetNumItems() )
    {
        return E_INVALIDARG;
    };

    struct _LISTNODE* pSearchNode = m_pHead;

    // If we have a valid index, find the node in the list
    for( unsigned int x = 0; x < uiIndex; ++x )
    {
        pSearchNode = pSearchNode->pNext;
    }

    // Set our current pointer to the node we found and update our index
    m_pCurrent = pSearchNode;
    m_uicCurrentItemIndex = uiIndex;

    return S_OK;
}


// Move to the next item in the list
template< class T > HRESULT CLinkedList< T >::MoveNext()
{
    if( m_pCurrent->pNext )
    {
        m_pCurrent = m_pCurrent->pNext;
        ++m_uicCurrentItemIndex;
    }

    return S_OK;
}


// Move to the prev item in the list
template< class T > HRESULT CLinkedList< T >::MovePrev()
{
    if( m_pCurrent->pPrev )
    {
        m_pCurrent = m_pCurrent->pPrev;
        --m_uicCurrentItemIndex;
    }

    return S_OK;
}


// Get a data item from our list by using an index value
template< class T > T& CLinkedList< T >::operator[]( unsigned int uiIndex )
{
    return GetNode( uiIndex );
}


// Get a the number of items in our list
template< class T > unsigned int CLinkedList< T >::GetNumItems( void ) const
{
    return m_uicNumItems;
}


#endif // _LINKEDLIST_H_