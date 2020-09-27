/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:        vector.c
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
* $Header:   G:/r_and_d/archives/common/utils/vector.c_v   1.3   11 Mar 1998 13:53:52   weiwang  $
******************************************************************************/
#include <stdlib.h>
#include <assert.h>

#include "VoxMem.h"

#include "vector.h"

#include "xvocver.h"

/*--------------------------------------------------
  private structure: VECTOR_ELEMENT
  --------------------------------------------------*/
typedef struct tagVECTOR_ELEMENT {
   void   *pMem;
   struct tagVECTOR_ELEMENT *pNext;
   struct tagVECTOR_ELEMENT *pLast;
} VECTOR_ELEMENT; 


/*--------------------------------------------------
  private structure: VECTOR
  --------------------------------------------------*/
typedef struct tagVECTOR {
   VECTOR_ELEMENT *pHead;
   VECTOR_ELEMENT *pTail;
} VECTOR;



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
void *vecInit(void)
{
   VECTOR *pVector=NULL;
   if(VOX_MEM_INIT(pVector,1,sizeof(VECTOR)))
     return NULL;
   else 
   {
      pVector->pHead=NULL;
      pVector->pTail=NULL;
      return pVector;
   } 
}

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
void vecFree(void *pvVector)
{
   VOX_MEM_FREE(pvVector)
}


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
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
void *vecAddElementAt(void *pvVector, void *pvElement, void *pMem, DIRECTION direction)
{
   VECTOR *pVector=(VECTOR *)pvVector;
   VECTOR_ELEMENT *pVectorElement=(VECTOR_ELEMENT *)pvElement;
   VECTOR_ELEMENT *pPrevious=NULL, *pInsert=NULL;

   pPrevious=(direction==FORWARDS)?pVectorElement->pNext:pVectorElement->pLast;

   if(VOX_MEM_INIT(pInsert, 1,sizeof(VECTOR_ELEMENT)))
     return NULL;
   else {
      if(direction==FORWARDS) {
         pInsert->pNext=pPrevious;
         pInsert->pLast=pVectorElement;
         pVectorElement->pNext=pInsert;
      } else {
         pInsert->pNext=pVectorElement;
         pInsert->pLast=pPrevious;
         pVectorElement->pLast=pInsert;
      }

      pInsert->pMem=pMem;

      if(!(pInsert->pNext))
         pVector->pTail=pInsert;
      if(!(pInsert->pLast))
         pVector->pHead=pInsert;

      return pInsert; /* user should check for NULL; */
   }
}


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
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/

void *vecAddElement(void *pvVector, void *pMem, DIRECTION direction)
{
   VECTOR *pVector=(VECTOR *)pvVector;
   VECTOR_ELEMENT *pVectorElement=NULL;

   if(!(pVector->pTail)) { 
      if(VOX_MEM_INIT(pVectorElement=pVector->pTail=pVector->pHead, 1,
         sizeof(VECTOR_ELEMENT))) 
         return NULL;
      else {
         pVectorElement->pLast=NULL;
         pVectorElement->pNext=NULL;
         pVectorElement->pMem=pMem;
      
         return (pVectorElement);
        }
   } else
      return(vecAddElementAt(pVector, (direction==FORWARDS)?pVector->pTail:pVector->pHead,
         pMem, direction));
}

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
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
void vecDelElement(void *pvVector, void *pvElement)
{
   VECTOR *pVector=(VECTOR *)pvVector;
   VECTOR_ELEMENT *pVectorElement=(VECTOR_ELEMENT *)pvElement;

   if(pVectorElement->pLast) 
      (pVectorElement->pLast)->pNext=pVectorElement->pNext;
   else
      pVector->pHead=pVectorElement->pNext;

   if(pVectorElement->pNext)
      (pVectorElement->pNext)->pLast=pVectorElement->pLast;
   else
      pVector->pTail=pVectorElement->pLast;

   pVectorElement->pMem=NULL;

   VOX_MEM_FREE(pVectorElement);

   return;
}


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
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
void vecRemoveAllElements(void *pvVector)
{
   VECTOR *pVector=(VECTOR *)pvVector;
   VECTOR_ELEMENT *pVectorElement=pVector->pHead, *pLastElement=NULL;

   if(pVectorElement) 
      do {
         pVectorElement->pMem=NULL;;
         pLastElement=pVectorElement;
         pVectorElement=pVectorElement->pNext;
         VOX_MEM_FREE(pLastElement);
      } while(pVectorElement);

   return;
}
 

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
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
void *vecFindElementFrom(void *pvElement, void *pMem, DIRECTION direction)
{
   VECTOR_ELEMENT *pVectorElement=(VECTOR_ELEMENT *)pvElement;
   
   while(pVectorElement->pMem!=pMem&&pVectorElement) 
      pVectorElement=(direction==FORWARDS)?pVectorElement->pNext:pVectorElement->pLast;

   return pVectorElement;
}


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
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
void *vecFindElement(void *pvVector, void *pMem, DIRECTION direction)
{
   return(vecFindElementFrom((direction==FORWARDS)?
      ((VECTOR *)pvVector)->pHead:((VECTOR *)pvVector)->pTail, pMem, direction));
}
 
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
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
void *vecGetNextElement(void *pvElement, DIRECTION direction)
{
   return((direction==FORWARDS)?((VECTOR_ELEMENT *)pvElement)->pNext:
      ((VECTOR_ELEMENT *)pvElement)->pLast);
}


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
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
void *vecGetData(void *pvElement)
{
   return(((VECTOR_ELEMENT *)pvElement)->pMem);
}


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
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
void vecSetData(void *pvElement, void *pMem)
{
   ((VECTOR_ELEMENT *)pvElement)->pMem=pMem;
   return;
}


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
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
void *vecGetFirstElement(void *pvVector)
{
   return(((VECTOR *)pvVector)->pHead);
}


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
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
void *vecGetLastElement(void *pvVector)
{
   return(((VECTOR *)pvVector)->pTail);
}


   



