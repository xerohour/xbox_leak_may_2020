/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:        vector.h
*
* Purpose:  Vector is meant to be an abstract approach to doubly linked list..
*           In it's raw form it may not have many applications due to the fact
*           that it doesn't support indexing.. But the user can easily extend on
*           this base to accomplish the indexing that they require. 
*
* Functions:   
*
* Author/Date:     Ilan Berci
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   G:/r_and_d/archives/common/utils/vector.h_v   1.2   11 Mar 1998 13:53:08   weiwang  $
******************************************************************************/

#ifndef __VECTOR
#define __VECTOR

typedef enum tagDIRECTION {BACKWARDS, FORWARDS} DIRECTION;

/* functions for initiating and freeing the vector */
/******************************************************************************
*
* Function:  vecInit()
*
* Action:    Initialize a vector structure
*
* Input:    none
*
* Output:   none
*
* Globals:  none
*
* Return:   pointer to the Vectore structure
*******************************************************************************/
void *vecInit(void);

/******************************************************************************
*
* Function:  vecFree()
*
* Action:    Free the memory of a Vector structure
*
* Input:    pvVector -- pointer to vector structure
*
* Output:   pvVector -- NULL pointer
*
* Globals:  none
*
* Return:   none
*******************************************************************************/
void vecFree(void *pvVector);


/* functions for inserting and removing elements */

/******************************************************************************
*
* Function:  vecAddElementAt()
*
* Action:    connect a new element to an existing element
*
* Input:    pvVector -- pointer to vector structure
*           pvElement -- pointer to element
*           pMem -- memory for insertion
*           direction -- FORWARDS or BACKWARDS
*
* Output:   none
*
* Globals:  none
*
* Return:   pointer for insertion
*******************************************************************************/
void *vecAddElementAt(void *pvVector, void *pvElement, void *pMem, DIRECTION direction);
/******************************************************************************
*
* Function:  vecAddElement()
*
* Action:    Add a new element at either the tail or the head of a Vector
*
* Input:    pvVector -- pointer to vector structure
*           pMem -- memory for insertion
*           direction -- FORWARDS or BACKWARDS
*
* Output:   none
*
* Globals:  none
*
* Return:   pointer for insertion
*******************************************************************************/
void *vecAddElement(void *pvVector, void *pMem, DIRECTION direction);

/******************************************************************************
*
* Function:  vecDelElement()
*
* Action:    remove a element from vector link
*
* Input:    pvVector -- pointer to vector structure
*           pvElement -- pointer to vector element
*
* Output:   pvVector -- updated link
*           pvElement -- NULL pointer
*
* Globals:  none
*
* Return:   none
*******************************************************************************/
void vecDelElement(void *pvVector, void *pvElement); 
/******************************************************************************
*
* Function:  vecRemoveAllElements()
*
* Action:    remove all elements from vector link
*
* Input:    pvVector -- pointer to vector structure
*
* Output:   pvVector -- updated link with no element
*
* Globals:  none
*
* Return:   none
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************/
void vecRemoveAllElements(void *pvVector);




/* functions for finding elements */
/******************************************************************************
*
* Function:  vecFindElementFrom()
*
* Action:    find the element with the same content as user provided
*
* Input:    pvElement -- pointer to vector element
*           pMem -- pointer to the memory user wants to compare with 
*           director -- BACKWARDS or FORWARDS for searching
*
* Output:   none
*
* Globals:  none
*
* Return:   pointer to vector element with match the content user provided
*******************************************************************************/
void *vecFindElementFrom(void *pvElement, void *pMem, DIRECTION direction);

/******************************************************************************
*
* Function:  vecFindElement()
*
* Action:   search an element from vector link
*
* Input:    pvVector -- pointer to vector 
*           pMem -- pointer to the memory user wants to compare with 
*           director -- BACKWARDS or FORWARDS for searching
*
* Output:   none
*
* Globals:  none
*
* Return:   pointer to vector element with match the content user provided
*******************************************************************************/
void *vecFindElement(void *pvVector, void *pMem, DIRECTION direction);

/******************************************************************************
*
* Function:  vecGetNextElement()
*

* Action:   Get the next element 
*
* Input:    pvElement -- pointer to vector element
*           director -- BACKWARDS or FORWARDS for searching
*
* Output:   none
*
* Globals:  none
*
* Return:   pointer to the next vector element 
*******************************************************************************/
void *vecGetNextElement(void *pvElement, DIRECTION direction);

/******************************************************************************
*
* Function:  vecGetFirstElement()
*
* Action:   Get the first vector element from the vector link
*
* Input:    pvVector -- pointer to a vector structure
*
* Output:   none
*
* Globals:  none
*
* Return:   pointer to the first vector element
*******************************************************************************/
void *vecGetFirstElement(void *pvVector);

/******************************************************************************
*
* Function:  vecGetLastElement()
*
* Action:   Get the last vector element from the vector link
*
* Input:    pvVector -- pointer to a vector structure
*
* Output:   none
*
* Globals:  none
*
* Return:   pointer to the last vector element
*******************************************************************************/
void *vecGetLastElement(void *pvVector);



/* functions for modifying element data */

/******************************************************************************
*
* Function:  vecGetData()
*
* Action:   Get the data contents from an element
*
* Input:    pvElement -- pointer to vector element
*
* Output:   none
*
* Globals:  none
*
* Return:   pointer to the data content
*******************************************************************************/
void *vecGetData(void *pvElement);

/******************************************************************************
*
* Function:  vecSetData()
*
* Action:   Set the data contents to a vector element
*
* Input:    pvElement -- pointer to vector element
*           pMem -- data contents
*
* Output:   none
*
* Globals:  none
*
* Return:   pointer to the updated vector element
*******************************************************************************/
void vecSetData(void *pvElement, void *pMem);


#endif /* __VECTOR */



