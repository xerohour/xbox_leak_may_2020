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

#define LINKEDLIST_APP_TITLE_NAME_A     "LIST"

#include <xtl.h>
#include <..\..\..\..\inc\xdbg.h>

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
    HRESULT DelCurrentNode();                                                   // Delete the node at the current index
    HRESULT EmptyList( void );                                                  // Empty all of the items out of our list
    HRESULT MoveTo( unsigned int uiIndex );                                     // Move to the specified item in our list via index
    BOOL MoveFirst();                                                           // Move to the first element in our list
    BOOL MoveLast();                                                            // Move to the last element in our list
    BOOL MoveNext();                                                            // Move to the next item in the list
    BOOL MovePrev();                                                            // Move to the prev item in the list
    unsigned int GetNumItems( void ) const;                                     // Get the number of items in our list
    void SetSortedList( BOOL bSorted ) { m_bSorted = bSorted; };                // Set this to have the list apply an insertion sort
    void SetDeleteDataItem( BOOL bDelItem ) { m_bDeleteDataItem = bDelItem; };  // Set this if the list needs to delete the dataitem
    T& GetCurrentNode() const;                                                  // Get a data item from the current node in our list
    T& GetNode( unsigned int uiIndex ) const;                                   // Get a data item from our list by using an index
    unsigned int GetCurrentIndex() const { return m_uicCurrentItemIndex; };     // Get the index of the current item within our list
    T& operator[]( unsigned int uiIndex ) const;                                // Get a data item from our list by using an index

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
    BOOL m_bDeleteDataItem;             // This needs to be TRUE if you are passing in a structure / item that was 'new'ed
    BOOL m_bSorted;                     // Used to determine if the list should be sorted

    // Private Methods
    HRESULT AddSorted( const T& item );
    HRESULT AddUnsorted( const T& item );
};


// Default Constructor.  Properties are initialized here
template< class T > CLinkedList< T >::CLinkedList( void )
: m_uicNumItems( 0 ),
  m_pHead( NULL ),
  m_pLast( NULL),
  m_pCurrent( NULL ),
  m_uicCurrentItemIndex( 0 ),
  m_bDeleteDataItem( FALSE),
  m_bSorted( FALSE )
{
}


// Destructor.  Memory cleanup is performed here
template< class T > CLinkedList< T >::~CLinkedList( void )
{
    // Empty all items from our list
    EmptyList();
}


// Add a node to our list in sorted order
template< class T > HRESULT CLinkedList< T >::AddSorted( const T& item )
{
    // Item to be added to our list
    struct _LISTNODE* pNewItem = new struct _LISTNODE;

    if( !pNewItem )
    {
        XDBGWRN( LINKEDLIST_APP_TITLE_NAME_A, " CLinkedList():AddSorted():Could not allocate memory!!" );

        return E_OUTOFMEMORY;
    }

    pNewItem->dataItem = item;
    pNewItem->pNext = NULL;
    pNewItem->pPrev = NULL;

    // Check to see if we have a head node.  If not, add the item there
    if( !m_pHead )
    {
        XDBGTRC( LINKEDLIST_APP_TITLE_NAME_A, " CLinkedList():AddSorted():Adding Head" );

        m_pCurrent = m_pLast = m_pHead = pNewItem;
        m_uicCurrentItemIndex = 0;  // Points at the head node

    }
    else // Add the node in the proper location of our list (sorted order)
    {
        struct _LISTNODE* pCurrent = m_pHead;
        struct _LISTNODE* pPrev = m_pHead;

        // Check to see if it's the head node that's replaced
        if( *(pNewItem->dataItem) < *(m_pHead->dataItem) )
        {
            XDBGTRC( LINKEDLIST_APP_TITLE_NAME_A, " CLinkedList():AddSorted():Replacing Head Node" );
            pNewItem->pNext = m_pHead;
            m_pHead->pPrev = pNewItem;
            m_pHead = pNewItem;
        }
        else // Add it to the middle, or the end of the list
        {
            while( pCurrent )
            {
                // If the item is not greater than the current item, we should insert it right before
                // the current item
                if( *(pNewItem->dataItem) < *(pCurrent->dataItem) )
                {
                    XDBGTRC( LINKEDLIST_APP_TITLE_NAME_A, " CLinkedList():AddSorted():Adding Middle" );
                    pPrev->pNext = pNewItem;            // Set the previous item's next pointer to our new item
                    pNewItem->pNext = pCurrent;         // Set the new item's next pointer to our current item
                    pNewItem->pPrev = pPrev;            // Set our new item's previous pointer to the previous node
                    pCurrent->pPrev = pNewItem;         // Set the current item's previous pointer to our new node

                    break;  // We no longer need to traverse the list
                }

                pPrev = pCurrent;
                pCurrent = pCurrent->pNext;
            }

            // If pCurrent is NULL, we need to add our new node at the end of the list
            if( NULL == pCurrent )
            {
                XDBGTRC( LINKEDLIST_APP_TITLE_NAME_A, " CLinkedList():AddSorted():Adding Last" );
                m_pLast->pNext = pNewItem;  // Add the new node to our list
                pNewItem->pPrev = m_pLast;  // Update the new nodes previous pointer
                m_pLast = pNewItem;         // Update our pLast pointer to point at the new last node
            }
        }
    }

    // Increment the number of items in our list
    ++m_uicNumItems;

    return S_OK;
}

// Add a node to our list without sorting it
template< class T > HRESULT CLinkedList< T >::AddUnsorted( const T& item )
{
    // Item to be added to our list
    struct _LISTNODE* pNewItem = new struct _LISTNODE;

    if( !pNewItem )
    {
        XDBGWRN( LINKEDLIST_APP_TITLE_NAME_A, " CLinkedList():AddUnsorted():Could not allocate memory!!" );

        return E_OUTOFMEMORY;
    }

    pNewItem->dataItem = item;
    pNewItem->pNext = NULL;
    pNewItem->pPrev = NULL;

    // Check to see if we have a head node.  If not, add the item there
    if( !m_pHead )
    {
        m_pCurrent = m_pLast = m_pHead = pNewItem;
        m_uicCurrentItemIndex = 0;  // Points at the head node

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

// Add a node to our list
template< class T > HRESULT CLinkedList< T >::AddNode( const T& item )
{
    if( m_bSorted )
        return AddSorted( item );
    else
        return AddUnsorted( item );
}


// Delete the node at the current index
template< class T > HRESULT CLinkedList< T >::DelCurrentNode()
{
    return DelNode( GetCurrentIndex() );
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
            m_pCurrent = m_pLast = m_pHead = NULL;
        else if( m_pCurrent == m_pHead ) // Deal with the 'Current' pointer if we have to
            m_pCurrent = m_pHead = m_pHead->pNext;       // If head is the only node, m_pHead->pNext will be NULL
        else // Only deal with the head node
            m_pHead = m_pHead->pNext;       // If head is the only node, m_pHead->pNext will be NULL

        // Check to see if our current point still points at anything, and if not,
        // make sure we set the Item Index to 0
        if( !m_pCurrent )
            m_uicCurrentItemIndex = 0;

        // Check to see if the new head node points at anything, and if so,
        // make sure the pPrev pointer is NULL
        if( m_pHead )
            m_pHead->pPrev = NULL;
    }
    else if( pSearchNode == m_pLast )   // Delete the last node in our list
    {
        // Check to see if our current node is pointing at the last node, and deal with it
        if( m_pCurrent == m_pLast )
        {
            m_pCurrent = m_pLast = pPrevNode;
            m_pCurrent->pNext = NULL;           // Set our next pointer to NULL
            --m_uicCurrentItemIndex;            // Decrement our current item index since we were pointing at the last node
        }
        else // Only deal with the last node pointer, and don't worry about the current node pointer
            m_pLast = pPrevNode;
    }
    else // Not the head node or the last node ( middle node! )
    {

        // Check to see if our current node is this node, and if so,
        // move it along as well
        if( pSearchNode == m_pCurrent )
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
            XDBGWRN( LINKEDLIST_APP_TITLE_NAME_A, "CLinkedList::EmptyList():Failed to delete node!!" );
        }
    }

    return hr;
}


// Get a data item from the current node in our list
template< class T > T& CLinkedList< T >::GetCurrentNode() const
{
    if( !m_pCurrent )
    {
        // This will only happen if the current pointer isn't pointing at a node
        // shame on the developer!!
        XDBGWRN( LINKEDLIST_APP_TITLE_NAME_A, "CLinkedList GetCurrentNode():BREAKING IN TO THE DEBUGGER!!" );
        XDBGWRN( LINKEDLIST_APP_TITLE_NAME_A, "CLinkedList GetCurrentNode()No node at the current pointer!!" );

        throw;
    }

    return m_pCurrent->dataItem;
}


// Get a data item from our list by using an index value
template< class T > T& CLinkedList< T >::GetNode( unsigned int uiIndex ) const
{
    // Make sure our index is valid.  If not, return an error
    if( uiIndex >= GetNumItems() )
    {
        XDBGWRN( LINKEDLIST_APP_TITLE_NAME_A, "CLinkedList GetNode():BREAKING IN TO THE DEBUGGER!!" );
        XDBGWRN( LINKEDLIST_APP_TITLE_NAME_A, "CLinkedList GetNode()No node at the specified index!!" );

        throw;
    };

    struct _LISTNODE* pSearchNode = m_pHead;

    // If we have a valid index, find the node in the list
    for( unsigned int x = 0; x < uiIndex; ++x )
    {
        pSearchNode = pSearchNode->pNext;
    }

    // Return the requested node to the user
    return pSearchNode->dataItem;
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


// Move to the first element in our list
template< class T > BOOL CLinkedList< T >::MoveFirst()
{
    if( GetNumItems() < 0 )
        return FALSE;

    m_pCurrent = m_pHead;
    m_uicCurrentItemIndex = 0;

    return TRUE;
}


// Move to the last element in our list
template< class T > BOOL CLinkedList< T >::MoveLast()
{
    if( GetNumItems() < 0 )
        return FALSE;

    m_pCurrent = m_pLast;
    m_uicCurrentItemIndex = GetNumItems() - 1;

    return TRUE;
}


// Move to the next item in the list
template< class T > BOOL CLinkedList< T >::MoveNext()
{
    if( m_pCurrent->pNext )
    {
        m_pCurrent = m_pCurrent->pNext;
        ++m_uicCurrentItemIndex;

        return TRUE;
    }

    return FALSE;
}


// Move to the prev item in the list
template< class T > BOOL CLinkedList< T >::MovePrev()
{
    if( m_pCurrent->pPrev )
    {
        m_pCurrent = m_pCurrent->pPrev;
        --m_uicCurrentItemIndex;

        return TRUE;
    }

    return FALSE;
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