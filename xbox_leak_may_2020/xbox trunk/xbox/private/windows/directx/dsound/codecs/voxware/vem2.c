/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:        vem2.c
*
* Purpose:     Main VEM functions
*
* Functions:  vemUnregisterState, ...
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
* $Header:   /export/phobos/pvcs/r_and_d/archives/common/vem/vem2.c_v   1.5   25 Mar 1998 14:23:12   weiwang  $
******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "VoxMem.h"

#include "vector.h"
#include "vem2Prm.h"

#include "vem2.h"

#include "xvocver.h"


#if VEM_DEFINED == 1
/*--------------------------------------------------
  private definitions: DISABLED and ENABLES
  --------------------------------------------------*/
typedef enum tagVEM_ENABLE{DISABLED, ENABLED} VEM_ENABLE;


/*--------------------------------------------------
  private structure: VEM_EXTENSION
  --------------------------------------------------*/
typedef struct tagVEM_EXTENSION {
   char        *pszKey;
   VEM_FUNC    *vemFunc;
   VEM_FUNC    *vemFuncFree;
   void        *vemFuncMem;
   void        *pAttachVec;
   VEM_ENABLE  enable; 
} VEM_EXTENSION;

/*--------------------------------------------------
  private structure: VEM_STATE
  --------------------------------------------------*/
typedef struct tagVEM_STATE {
   char        *pszKey;
   void        *pAttachVec;
   VEM_ENABLE  enable;
} VEM_STATE;

/*--------------------------------------------------
  private structure: VEM_ATTACH
  --------------------------------------------------*/
typedef struct tagVEM_ATTACH {
   unsigned short priority;
   VEM_EXTENSION *pVemExtension;
   VEM_STATE *pVemState;
} VEM_ATTACH;


/******************************************************************************
*
* Function:  vemUnregisterState
*
* Action:    Erase the existence of the state from the Vem memory handle
*
* Input:    hVemMemHandle -- main Vem memory handle
*           stateKey -- key to locate state
*
* Output:   stateKey -- updated state key
*
* Globals:  none
*
* Return:   NULL key
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:  Since all states are automatically unregistered at cleanup, this 
*            function need not be called unless the user intenses to do so. All 
*            extensions that are associated with this state will have their ties
*            removed. Please note that any state can be temporarily disabled
*            with vemDisableState().
*
* Concerns/TBD:
******************************************************************************/
VEM_KEY vemUnregisterState(const void *hVemMemHandle, VEM_KEY stateKey)
{
   VEM_ATTACH        *pVemAttach       = NULL;
   VEM_KEY            extensionKey     = NULL;
   void              *pElement;
   VEM2              *pVemMemHandler   = (VEM2 *)hVemMemHandle;

   VEM_STATE         *pVemState        = (VEM_STATE *)vecGetData((void *)stateKey);
 
   if(pVemState) {
      if(pVemState->pAttachVec) {

         while(0 != (pElement=vecGetFirstElement(pVemState->pAttachVec))) {
            pVemAttach=(VEM_ATTACH *)vecGetData(pElement);
            extensionKey=(VEM_KEY)vecFindElement(pVemMemHandler->pVemExtensions, 
               pVemAttach->pVemExtension, FORWARDS);

            vemDetachExtension(stateKey, extensionKey);
         }

         vecFree(pVemState->pAttachVec);
         pVemState->pAttachVec=NULL;
      }
      vecDelElement(pVemMemHandler->pVemStates, stateKey);
      VOX_MEM_FREE(pVemState->pszKey);
      VOX_MEM_FREE(pVemState);
   }

   return NULL;
}


/******************************************************************************
*
* Function:  vemRegisterState
*
* Action:    Register a state and should be called at a non critical time such
*            as in the initialization sequence
*
* Input:    hVemMemHandle -- main Vem memory handle
*           pszKey -- unique state identifier string
*
* Output:   none
*
* Globals:  none
*
* Return:   the key (or pointer) to quickly locate state
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
VEM_KEY vemRegisterState(const void *hVemMemHandle, const char *pszKey)
{
   VEM_STATE         *pVemState        = NULL;
   VEM_KEY           *pKey             = NULL;
   VEM2              *pVemMemHandler   = (VEM2 *)hVemMemHandle;

   /* initialize main vem state memory */
   if(VOX_MEM_INIT(pVemState,1,sizeof(VEM_STATE)))
       return NULL;
   else {
      pVemState->pszKey=NULL;
      pVemState->pAttachVec=NULL;
      pVemState->enable=ENABLED;
   }
      

   /* initialize and store the vector key */
   if(!(pKey=(VEM_KEY)vecAddElement(pVemMemHandler->pVemStates, pVemState, FORWARDS))) {
      VOX_MEM_FREE(pVemState);
      return NULL;
   }

   /* initialize the string key */
   if(VOX_MEM_INIT(pVemState->pszKey,strlen(pszKey)+1,sizeof(char))) 
      return(vemUnregisterState(hVemMemHandle, pKey));
   else
      strcpy(pVemState->pszKey,pszKey);

   /* initialize the state's extension vector */
   if(!(pVemState->pAttachVec=vecInit())) 
      return(vemUnregisterState(hVemMemHandle, pKey));

   return(pKey);
}


/******************************************************************************
*
* Function:  vemServiceState()
*
* Action:    This function services all states that were previously registered 
*            and enabled.
*
* Input:    stateKey -- key to locate state
*
* Output:   stateKey -- updated state key
*
* Globals:  none
*
* Return:   0: succeed
*           1: failed
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

unsigned short vemServiceState(VEM_KEY stateKey)
{
   VEM_STATE         *pVemState        = NULL;
   VEM_EXTENSION     *pVemExtension    = NULL;
   void              *pElement         = NULL;

   if(!(pVemState=(VEM_STATE *)vecGetData((void *)stateKey)))
      return 1;

   if(pVemState->enable==DISABLED)
      return 1;

   pElement = vecGetFirstElement(pVemState->pAttachVec);

   while(pElement) {
      pVemExtension=((VEM_ATTACH*)vecGetData(pElement))->pVemExtension;
      if(pVemExtension->enable==ENABLED)
         (pVemExtension->vemFunc)(pVemExtension->vemFuncMem, stateKey);

      pElement=vecGetNextElement(pElement, FORWARDS);
   }
   return 0;
}

/******************************************************************************
*
* Function:  vemDisableState()
*
* Action:    This function will temporarily disable a state. All extensions 
*            attached to this state will still keep their relationship.
*
* Input:    stateKey -- key to locate state
*
* Output:   stateKey -- key with state disabled
*
* Globals:  none
*
* Return:   0: succeed
*           1: failed
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

unsigned short vemDisableState(VEM_KEY stateKey)
{
   VEM_STATE *pVemState=vecGetData((void *)stateKey);

   if(!pVemState)
      return 1;
   else
      pVemState->enable=DISABLED;
   return 0;
}


/******************************************************************************
*
* Function:  vemEnableState()
*
* Action:    This function will enable a state.
*
* Input:    stateKey -- key to locate state
*
* Output:   stateKey -- Key with state enabled
*
* Globals:  none
*
* Return:   0: succeed
*           1: failed
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

unsigned short vemEnableState(VEM_KEY stateKey)
{
   VEM_STATE *pVemState=vecGetData((void *)stateKey);

   if(!pVemState)
      return 1;
   else
      pVemState->enable=ENABLED;
   return 0;
}

/******************************************************************************
*
* Function:  vemGetState()
*
* Action:    Get the key for a previously registered state
*
* Input:    hVemMemHandle -- main Vem memory handle
*           pszKey -- state's string identifer
*
* Output:   none
*
* Globals:  none
*
* Return:   key to locate state
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:  This function should only be used when absolutely necessary as it
*            is CPU intensive.
*
* Concerns/TBD:
******************************************************************************/

VEM_KEY vemGetState(const void *hVemMemHandle, const char *pszKey)
{
   void              *pVemElement      = NULL;
   VEM2              *pVemMemHandler   = (VEM2 *)hVemMemHandle;

   pVemElement = vecGetFirstElement(pVemMemHandler->pVemStates);
   while(pVemElement) {
      if(!strcmp(pszKey, ((VEM_STATE *)vecGetData(pVemElement))->pszKey))
         break; /* match! */
      pVemElement=vecGetNextElement(pVemElement, FORWARDS);
   }
   return ((VEM_KEY)pVemElement); /* could be null if it wasn't found */
}



/******************************************************************************
*
* Function:  vemUnregisterExtension()
*
* Action:    This function can permanently removes an extension from the main Vem 
*            memory handle. 
*
* Input:    hVemMemHandle -- main Vem memory handle
*           extensionKey -- key to locate extension          
*
* Output:   extensionKey -- updated extension key
*
* Globals:  none
*
* Return:   a NULL key
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:  If user intends to re-invoke the extension, then it whould be 
*            preferable to just temporarily disable it with vemDisableExtension().
*
* Concerns/TBD:
******************************************************************************/
void *vemUnregisterExtension(const void *hVemMemHandle, VEM_KEY extensionKey)
{
   VEM_EXTENSION     *pVemExtension    = NULL;
   VEM_ATTACH        *pVemAttach       = NULL;
   VEM_KEY            stateKey         = NULL;
   void              *pElement         = NULL;
   VEM2              *pVemMemHandler   = (VEM2 *)hVemMemHandle;
   
   if(!extensionKey)
      return NULL;

   pVemExtension=(VEM_EXTENSION *)vecGetData((void *)extensionKey);

   if(pVemExtension) {
      if(pVemExtension->pAttachVec) {

         while(0 != (pElement=vecGetFirstElement(pVemExtension->pAttachVec))) {
            pVemAttach=(VEM_ATTACH *)vecGetData(pElement);
            stateKey=(VEM_KEY)vecFindElement(pVemMemHandler->pVemStates, 
               pVemAttach->pVemState, FORWARDS);

            vemDetachExtension(stateKey, extensionKey);
         }

         vecFree(pVemExtension->pAttachVec);
         pVemExtension->pAttachVec=NULL;
      }
      /* call the extension's termination routine */
      if(pVemExtension->vemFuncFree)
         (pVemExtension->vemFuncFree)(pVemExtension->vemFuncMem, extensionKey);

      vecDelElement(pVemMemHandler->pVemExtensions, extensionKey);
      VOX_MEM_FREE(pVemExtension->pszKey);
      VOX_MEM_FREE(pVemExtension);
   }
   return NULL;
}


/******************************************************************************
*
* Function:  vemRegisterExtension()
*
* Action:    Register the extension with the main Vem memory handle. It's enabled
*            by default and the key that is returned is need in order to attach
*            it to a state.
*
* Input:    hVemMemHandle -- main Vem memory handle
*           pszKey -- state's string identifier
*           pVemFunc -- extension's function
*           pVemFuncFree -- extension's free function
*           pVemFuncMem -- void pointer to extension's memory block
*           
* Output:   none
*
* Globals:  none
*
* Return:   key to locate extension
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
VEM_KEY vemRegisterExtension(const void *hVemMemHandle, const char *pszKey, 
                             VEM_FUNC *pVemFunc, VEM_FUNC *pVemFuncFree, 
                             void *pVemFuncMem)
{
   VEM_EXTENSION     *pVemExtension    = NULL;
   VEM_KEY           *pKey             = NULL;
   VEM2              *pVemMemHandler   = (VEM2 *)hVemMemHandle;

   if(VOX_MEM_INIT(pVemExtension,1,sizeof(VEM_EXTENSION)))
       return NULL;
   else {
      pVemExtension->pszKey=NULL;
      pVemExtension->vemFunc=pVemFunc;
      pVemExtension->vemFuncFree=pVemFuncFree;
      pVemExtension->vemFuncMem=pVemFuncMem;
      pVemExtension->pAttachVec=NULL;
      pVemExtension->enable=ENABLED;
   }

   if(!(pKey=vecAddElement(pVemMemHandler->pVemExtensions, pVemExtension, FORWARDS))) {
      VOX_MEM_FREE(pVemExtension);
      return NULL;
   }

   /* initialize the string key */
   if(VOX_MEM_INIT(pVemExtension->pszKey,strlen(pszKey)+1,sizeof(char))) 
      return(vemUnregisterExtension(hVemMemHandle, pKey));
   else
      strcpy(pVemExtension->pszKey,pszKey);

   /* initialize the extension's state vector */
   if(!(pVemExtension->pAttachVec=vecInit())) 
      return(vemUnregisterExtension(hVemMemHandle, pKey));

   /* initialize and store the vector key */
   return(pKey);
}


/******************************************************************************
*
* Function:  vemDisableExtension()
*
* Action:   This function will temporarily disable the extension. It will still
*           be registered with the Vem memory handle.
*
* Input:    extensionKey -- key to locate extension
*           
* Output:   extensionKey -- key with the enable flag turned off
*
* Globals:  none
*
* Return:   0: succeed
*           1: failed
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

unsigned short vemDisableExtension(VEM_KEY extensionKey)
{
   VEM_EXTENSION *pVemExtension=(VEM_EXTENSION *)vecGetData((void *)extensionKey);

   if(!pVemExtension)
      return 1;
   else
      pVemExtension->enable=DISABLED;

   return 0;
}

/******************************************************************************
*
* Function:  vemEnalbeExtension()
*
* Action:   If the extension was disabled, the extension can be enable again by
*           calling this function
*
* Input:    extensionKey -- key to locate extension
*           
* Output:   extensionKey -- Key with the enable flag turned on
*
* Globals:  none
*
* Return:   0: succeed
*           1: failed
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

unsigned short vemEnableExtension(VEM_KEY extensionKey)
{
   VEM_EXTENSION *pVemExtension=(VEM_EXTENSION *)vecGetData((void *)extensionKey);

   if(!pVemExtension)
      return 1;
   else
      pVemExtension->enable=ENABLED;

   return 0;
}

/******************************************************************************
*
* Function:  vemGetExtension()
*
* Action:    If the extensions is not known, then this function can be called 
*            to retrieve it.
*
* Input:    hVemMemHandle -- main Vem memory handle
*           pszKey -- state's string identifier
*           
* Output:   none
*
* Globals:  none
*
* Return:   key to locate extension
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:  This function is costly and should only be called when absolutely
*            neccesary, preferably during a non-time critical segment of the 
*            program.
*
* Concerns/TBD:
******************************************************************************/
VEM_KEY vemGetExtension(const void *hVemMemHandle, const char *pszKey)
{
   void              *pVemElement      = NULL;
   VEM2              *pVemMemHandler   = (VEM2 *)hVemMemHandle;

   assert(hVemMemHandle != NULL);

   pVemElement = vecGetFirstElement(pVemMemHandler->pVemExtensions);
   while(pVemElement) {
      if(!strcmp(pszKey, ((VEM_EXTENSION *)vecGetData(pVemElement))->pszKey))
         break; /* match! */
      pVemElement=vecGetNextElement(pVemElement, FORWARDS);
   }
   return pVemElement; /* could be null if it wasn't found */
}


/******************************************************************************
*
* Function:  vemAttachExtension()
*
* Action:    Attach the state with the extension
*
* Input:    stateKey -- key to locate state
*           extensionKey -- key to locate extension
*           priority -- priority level of attachment
*
* Output:   stateKey -- updated state key
*           extensionKey -- updated extension Key
*
* Globals:  none
*
* Return:   0: succeed
*           1: failed
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
unsigned short vemAttachExtension(VEM_KEY stateKey, VEM_KEY extensionKey,
                                  const unsigned short priority)
{
   VEM_STATE         *pVemState        = NULL;
   VEM_EXTENSION     *pVemExtension    = NULL;
   VEM_ATTACH        *pVemAttach       = NULL;
   VEM_ATTACH        *pVemAttachSeek   = NULL;

   pVemState=(VEM_STATE *)vecGetData((void*)stateKey);
   pVemExtension=(VEM_EXTENSION *)vecGetData((void*)extensionKey);

   if(VOX_MEM_INIT(pVemAttach,1,sizeof(VEM_ATTACH)))
      return 1;
   else {
      pVemAttach->priority=priority;
      pVemAttach->pVemExtension=pVemExtension;
      pVemAttach->pVemState=pVemState;
   }

   /* adding the state to the extension's list */
   pVemAttachSeek=vecGetFirstElement(pVemExtension->pAttachVec);
      
   while(pVemAttachSeek) {
      if(((VEM_ATTACH *)vecGetData(pVemAttachSeek))->priority<priority) {
         pVemAttachSeek=vecAddElementAt(pVemExtension->pAttachVec, pVemAttachSeek, 
            pVemAttach, BACKWARDS);
         break;
      }
      pVemAttachSeek=vecGetNextElement(pVemAttachSeek, FORWARDS);
   }
   if(!pVemAttachSeek)
      if(!vecAddElement(pVemExtension->pAttachVec, pVemAttach, FORWARDS))
         return 1;

   /* adding the extension to the state's list */
   pVemAttachSeek=vecGetFirstElement(pVemState->pAttachVec);
      
   while(pVemAttachSeek) {
      if(((VEM_ATTACH *)vecGetData(pVemAttachSeek))->priority<priority) {
         pVemAttachSeek=vecAddElementAt(pVemState->pAttachVec, pVemAttachSeek, 
            pVemAttach, BACKWARDS);
         break;
      }
      pVemAttachSeek=vecGetNextElement(pVemAttachSeek, FORWARDS);
   }

   /* either the list was empty or the priorities were too high.. insert at end */
   if(!pVemAttachSeek)
      if(!vecAddElement(pVemState->pAttachVec, pVemAttach, FORWARDS))
         return 1;

      return 0;
}


/******************************************************************************
*
* Function:  vemDetachExtension()
*
* Action:    Detach the state from the extension
*
* Input:    stateKey -- key to locate state
*           extensionKey -- key to locate extension
*
* Output:   stateKey -- updated state key
*           extensionKey -- updated extension key
*
* Globals:  none
*
* Return:   0: succeed
*           1: failed
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
unsigned short vemDetachExtension(VEM_KEY stateKey, VEM_KEY extensionKey)
{
   VEM_STATE         *pVemState        = NULL;
   VEM_EXTENSION     *pVemExtension    = NULL;
   VEM_ATTACH        *pVemAttach       = NULL;
   void              *pElement         = NULL;

   pVemState=(VEM_STATE *)vecGetData((void*)stateKey);
   pVemExtension=(VEM_EXTENSION *)vecGetData((void*)extensionKey);

   /* detaching state from extension */
   pElement=vecGetFirstElement(pVemExtension->pAttachVec);
   while(pElement) {
      pVemAttach=(VEM_ATTACH *)vecGetData(pElement);
      if(pVemAttach->pVemState==pVemState) {
         vecDelElement(pVemExtension->pAttachVec, pElement);
         if(0 != (pElement=vecFindElement(pVemState->pAttachVec, pVemAttach, FORWARDS)))
            vecDelElement(pVemState->pAttachVec,pElement);
         break;
      }
      pElement=vecGetNextElement(pElement, FORWARDS);
   }

   if(!pElement)
      return 1; /* couldn't find it */

   VOX_MEM_FREE(pVemAttach);
   
   return 0;
}


/******************************************************************************
*
* Function:  vemGetExtensionFunc()
*
* Action:   Retrieve the extension function
*
* Input:    extensionKey -- key to locate eaxtension
*           
* Output:   none
*
* Globals:  none
*
* Return:   pointer to extension's callback function
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

VEM_FUNC *vemGetExtensionFunc(const VEM_KEY extensionKey)
{
   VEM_EXTENSION *pVemExtension=(VEM_EXTENSION *)vecGetData((void *)extensionKey);

   return((pVemExtension)?pVemExtension->vemFunc:NULL);
}


/******************************************************************************
*
* Function:  vemSetExtensionFunc()
*
* Action:   Set the extension's callback function
*
* Input:    extensionKey -- key to locate extension
*           vemFunc -- pointer to extension's callback function
*           
* Output:   extensionKey -- updated extension key
*
* Globals:  none
*
* Return:   0: succeed
*           1: failed
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
unsigned short vemSetExtensionFunc(VEM_KEY extensionKey,
                                   VEM_FUNC *vemFunc) 
{
   VEM_EXTENSION *pVemExtension=(VEM_EXTENSION *)vecGetData((void *)extensionKey);

   if(pVemExtension) {
      pVemExtension->vemFunc=vemFunc;
      return 0;
   } else
      return 1;
}

/******************************************************************************
*
* Function:  vemGetExtensionData()
*
* Action:   Return the extension's memory
*
* Input:    extensionKey -- key to locate eaxtension
*           
* Output:   none
*
* Globals:  none
*
* Return:   pointer to extension's memory block
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
void *vemGetExtensionData(const VEM_KEY extensionKey)
{
   VEM_EXTENSION *pVemExtension=(VEM_EXTENSION *)vecGetData((void *)extensionKey);

   return((pVemExtension)?pVemExtension->vemFuncMem:NULL);
}


/******************************************************************************
*
* Function:  vemSetExtensionData()
*
* Action:   Set the extension's memory
*
* Input:    extensionKey -- key to locate extension
*           vemFuncMem -- pointer to extension's memory block
*           
* Output:   extensionKey -- updated extension key
*
* Globals:  none
*
* Return:   0: succeed 
*           1: failed
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
unsigned short vemSetExtensionData(VEM_KEY extensionKey, void *vemFuncMem) 
{
   VEM_EXTENSION *pVemExtension=(VEM_EXTENSION *)vecGetData((void *)extensionKey);

   if(pVemExtension) {
      pVemExtension->vemFuncMem=vemFuncMem;
      return 0;
   } else
      return 1;
}


/******************************************************************************
*
* Function:  vemFree()
*
* Action:    Destroys the current instance of Vem.
*
* Input:    hVemMainHandle -- Vem handler
*
* Output:   hVemMainHandle -- NULL pointer
*
* Globals:   none
*
* Return:    NULL pointer
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
void *vemFree(void *hVemMainHandle)
{
   VEM_KEY            pElement         = NULL;
   VEM_MAIN_STRUCT   *pVemMainStruct   = NULL;
   VEM2              *pVemMemHandler   = NULL;

   if(hVemMainHandle)
      pVemMainStruct   = (VEM_MAIN_STRUCT *)hVemMainHandle;
   pVemMemHandler = pVemMainStruct->pMemoryHandler;

   if(0 != (pVemMemHandler=pVemMainStruct->pMemoryHandler)) {
      if(pVemMemHandler->pVemStates) {
         while(0 != (pElement=(VEM_KEY)vecGetFirstElement(pVemMemHandler->pVemStates))) 
            vemUnregisterState(pVemMemHandler, pElement);
         vecFree(pVemMemHandler->pVemStates);
         pVemMemHandler->pVemStates=NULL;
      }
      if(pVemMemHandler->pVemExtensions) {
         while(0 != (pElement=vecGetFirstElement(pVemMemHandler->pVemExtensions))) 
            vemUnregisterExtension(pVemMemHandler, pElement);
         vecFree(pVemMemHandler->pVemExtensions);
         pVemMemHandler->pVemExtensions=NULL;
      }
      if(pVemMemHandler->pVemParameters) {
         while(0 != (pElement=vecGetFirstElement(pVemMemHandler->pVemParameters)))
            vemUnregisterParameter(pVemMemHandler, pElement);
         vecFree(pVemMemHandler->pVemParameters);
         pVemMemHandler->pVemParameters=NULL;
      }
      if(pVemMemHandler->pFunctions)
         vecFree(pVemMemHandler->pFunctions);
      pVemMemHandler->pFunctions=NULL;

      VOX_MEM_FREE(pVemMainStruct->pMemoryHandler);
   }

   VOX_MEM_FREE(pVemMainStruct->pVemMethodStruct);

   VOX_MEM_FREE(hVemMainHandle);

   return hVemMainHandle;
}


/******************************************************************************
*
* Function:  vemInit()
*
* Action:    Initilaize Vem memory and extensions. It passes back it's memory
*            handle back to the user which should be checked for a NULL value
*            in case of insufficient memory.
*
* Input:    none
*
* Output:   none
*
* Globals:   none
*
* Return:    Vem memory handler
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

void *vemInit()
{
   VEM_MAIN_STRUCT   *pVemMainStruct   = NULL;
   VEM2              *pMemoryHandler   = NULL;
   VEM_METHOD_STRUCT *pVemMethodStruct = NULL;

   if(VOX_MEM_INIT(pVemMainStruct,1,sizeof(VEM_MAIN_STRUCT)))
      return(vemFree(pVemMainStruct));

   if(VOX_MEM_INIT(pMemoryHandler = pVemMainStruct->pMemoryHandler,1,sizeof(VEM2)))
      return(vemFree(pVemMainStruct));
   else {
      if(!(pMemoryHandler->pVemStates=vecInit())) 
         return(vemFree(pVemMainStruct));

      if(!(pMemoryHandler->pVemExtensions=vecInit())) 
         return(vemFree(pVemMainStruct));

      if(!(pMemoryHandler->pVemParameters=vecInit())) 
         return(vemFree(pVemMainStruct));

      if(!(pMemoryHandler->pFunctions=vecInit())) 
         return(vemFree(pVemMainStruct));
   }
      

   if(VOX_MEM_INIT(pVemMethodStruct = pVemMainStruct->pVemMethodStruct,1,
      sizeof(VEM_METHOD_STRUCT))) 
      return(vemFree(pVemMainStruct));

   /* Put in the callbacks for the extrinsic interface */

   /******* Extensions *******/
   pVemMethodStruct->lpfnVemUnregisterState     = vemUnregisterState;
   pVemMethodStruct->lpfnVemRegisterState       = vemRegisterState;
   pVemMethodStruct->lpfnVemServiceState        = vemServiceState;
   pVemMethodStruct->lpfnVemDisableState        = vemDisableState;
   pVemMethodStruct->lpfnVemEnableState         = vemEnableState;
   pVemMethodStruct->lpfnVemGetState            = vemGetState;

   pVemMethodStruct->lpfnVemShowStates          = vemShowStates;

   /******* Extensions *******/
   pVemMethodStruct->lpfnVemUnregisterExtension = vemUnregisterExtension;
   pVemMethodStruct->lpfnVemRegisterExtension   = vemRegisterExtension;
   pVemMethodStruct->lpfnVemGetExtension        = vemGetExtension;
   pVemMethodStruct->lpfnVemDisableExtension    = vemDisableExtension;
   pVemMethodStruct->lpfnVemEnableExtension     = vemEnableExtension;
   pVemMethodStruct->lpfnVemAttachExtension     = vemAttachExtension;
   pVemMethodStruct->lpfnVemDetachExtension     = vemDetachExtension;
   pVemMethodStruct->lpfnVemGetExtensionFunc    = vemGetExtensionFunc;
   pVemMethodStruct->lpfnVemSetExtensionFunc    = vemSetExtensionFunc;
   pVemMethodStruct->lpfnVemGetExtensionData    = vemGetExtensionData;
   pVemMethodStruct->lpfnVemSetExtensionData    = vemSetExtensionData;
   pVemMethodStruct->lpfnVemShowExtensions      = vemShowExtensions;

   /******* Parameters *******/
   pVemMethodStruct->lpfnVemUnregisterParameter = vemUnregisterParameter;
   pVemMethodStruct->lpfnVemRegisterArray       = vemRegisterArray;
   pVemMethodStruct->lpfnVemRegisterTable       = vemRegisterTable;
   pVemMethodStruct->lpfnVemRegisterVariable    = vemRegisterVariable;
   pVemMethodStruct->lpfnVemRegisterStruct      = vemRegisterStruct;
   pVemMethodStruct->lpfnVemRegisterConstFloat  = vemRegisterConstFloat;
   pVemMethodStruct->lpfnVemRegisterConstShort  = vemRegisterConstShort;
   pVemMethodStruct->lpfnVemGetParameter        = vemGetParameter;
   pVemMethodStruct->lpfnVemGetParameterData    = vemGetParameterData;
   pVemMethodStruct->lpfnVemSetParameterData    = vemSetParameterData;
   pVemMethodStruct->lpfnVemShowParameters      = vemShowParameters;
      
   return((void *)pVemMainStruct);
}

/**************
** Diagnostics
**************/
#if VEM_TRACE==1

#include <stdio.h>

/******************************************************************************
*
* Function:  vemShowStates()
*
* Action:    Print out a list of all states currently registered under the current
*            Vem memory handle. Every state that has one or more extensions attached
*            will also be listed.
*
* Input:    hVemMemHandle -- main Vem memory handle
*
* Output:   print the messages on stdout
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

void vemShowStates(const void *hVemMemHandle) 
{
   VEM_STATE         *pVemState        = NULL;
   VEM_ATTACH        *pVemAttach       = NULL;
   void              *pVemElement      = NULL, *pVemElement2 = NULL;
   VEM2              *pVemMemHandler   = (VEM2 *)hVemMemHandle;

   pVemElement = vecGetFirstElement(pVemMemHandler->pVemStates);
   if(pVemElement)
      printf("\n\nStates registered: ");
   else
      printf("\nNo states registered ");
   while(pVemElement) {
      pVemState=(VEM_STATE *)vecGetData(pVemElement);
      printf("\n   \"%s\"", pVemState->pszKey);
      printf("\n      Status: %s", (pVemState->enable)?"enabled":"disabled");
      printf("\n      Attached Extensions: ");
      pVemElement2=vecGetFirstElement(pVemState->pAttachVec);
      while(pVemElement2) {
         pVemAttach=(VEM_ATTACH *)vecGetData(pVemElement2);
         printf("\n         \"%s\"", (pVemAttach->pVemExtension)->pszKey);
         pVemElement2=vecGetNextElement(pVemElement2, FORWARDS);
      }
      pVemElement=vecGetNextElement(pVemElement, FORWARDS);
   }
   return;
} 


/******************************************************************************
*
* Function:  vemShowExtensions()
*
* Action:    Show all extensions and every extension's states attachment
*
* Input:    hVemMemHandle -- main Vem memory handle
*
* Output:   print the messages on stdout
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

void vemShowExtensions(const void *hVemMemHandle) 
{
   VEM_EXTENSION     *pVemExtension    = NULL;
   VEM_ATTACH        *pVemAttach       = NULL;
   void              *pVemElement      = NULL, *pVemElement2 = NULL;
   VEM2              *pVemMemHandler   = (VEM2 *)hVemMemHandle;

   pVemElement = vecGetFirstElement(pVemMemHandler->pVemExtensions);
   if(pVemElement)
      printf("\n\nExtensions registered: ");
   else
      printf("\nNo extensions registered ");
   while(pVemElement) {
      pVemExtension=(VEM_EXTENSION *)vecGetData(pVemElement);
      printf("\n   \"%s\"", pVemExtension->pszKey);
      printf("\n      Status:          %s", (pVemExtension->enable)?"enabled":"disabled");
      printf("\n      Function:        0x%X",
             (unsigned int) pVemExtension->vemFunc);
      printf("\n      Function memory: 0x%X",
             (unsigned int) pVemExtension->vemFuncMem);
      printf("\n      Attached states: ");
      pVemElement2=vecGetFirstElement(pVemExtension->pAttachVec);
      while(pVemElement2) {
         pVemAttach=(VEM_ATTACH *)vecGetData(pVemElement2);
         printf("\n         \"%s\"", (pVemAttach->pVemState)->pszKey);
         pVemElement2=vecGetNextElement(pVemElement2, FORWARDS);
      }
      pVemElement=vecGetNextElement(pVemElement, FORWARDS);
   }
   return;   
}

#else /* VEM_TRACE */

/* if VEM_TRACE is off, there will be warnings for unreferenced paramenters. 
   Please ignore them */
void vemShowStates(const void *hVemMemHandle) {return;}
void vemShowExtensions(const void *hVemMemHandle) {return;}

#endif /* VEM_TRACE */

#endif /* VEM_DEFINED */
