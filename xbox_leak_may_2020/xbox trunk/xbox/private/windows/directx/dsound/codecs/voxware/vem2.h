/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:        vem2.h
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
* $Header:   /export/phobos/pvcs/r_and_d/archives/common/vem/vem2.h_v   1.6   15 Apr 1998 16:53:22   weiwang  $
******************************************************************************/

#ifndef __VEM2
#define __VEM2

/***************************************************
* usefull defines to get the handles 
***************************************************/
#define GET_VEM_MEMORY_HANDLE(a) (((VEM_MAIN_STRUCT *)VCI_GET_VEM_HANDLE(a))->pMemoryHandler )
#define GET_VEM_METHOD_HANDLE(a) (((VEM_MAIN_STRUCT *)VCI_GET_VEM_HANDLE(a))->pVemMethodStruct)

/* switch for DSP's */
#define VEM_DEFINED 1

/* switch to turn on diagnostics */
#define VEM_TRACE 0

#include <stdlib.h>

/***************************************************
* VEM_KEY :
****************************************************/
#ifndef __VEM_KEY
#define __VEM_KEY
typedef void * VEM_KEY;
#endif /*  __VEM_KEY */

typedef unsigned short (VEM_FUNC)(void *, VEM_KEY);


/***************************************************
* public structure: PARAMETER_TYPE
****************************************************/  
#ifndef __VEM_PARAMETER_TYPE
#define __VEM_PARAMETER_TYPE
typedef enum tagPARAMETER_TYPE {
   VEM_VOID,
   VEM_CHAR,
   VEM_U_CHAR,
   VEM_SHORT,
   VEM_U_SHORT,
   VEM_INT,
   VEM_U_INT,
   VEM_LONG,
   VEM_U_LONG,
   VEM_FLOAT,
   VEM_DOUBLE
} PARAMETER_TYPE;
#endif /* __VEM_PARAMETER_TYPE */

/***************************************************
* public structure: VEM_METHOD_STRUCT
****************************************************/
typedef struct tagVEM_METHOD_STRUCT {
   /******* States *******/
   VEM_KEY (*lpfnVemUnregisterState)(const void *hVemMemHandle, VEM_KEY stateKey);
   VEM_KEY (*lpfnVemRegisterState)(const void *hVemMemHandle, const char *pszKey);
   unsigned short (*lpfnVemServiceState)(VEM_KEY stateKey);
   unsigned short (*lpfnVemDisableState)(VEM_KEY stateKey);
   unsigned short (*lpfnVemEnableState)(VEM_KEY  stateKey);
   VEM_KEY (*lpfnVemGetState)(const void *hVemMemHandle, const char *pszKey);

   void (*lpfnVemShowStates)(const void *hVemMemHandle);

   /******* Extensions *******/
   VEM_KEY (*lpfnVemUnregisterExtension)(const void *hVemMemHandle, VEM_KEY extensionKey);
   VEM_KEY (*lpfnVemRegisterExtension)(const void *hVemMemHandle, const char *pszKey, 
      VEM_FUNC *pVemFunc, VEM_FUNC *pVemFuncFree, void *pVemFuncMem);
   VEM_KEY (*lpfnVemGetExtension)(const void *hVemMemHandle, const char *pszKey);
   unsigned short (*lpfnVemDisableExtension)(VEM_KEY extensionKey);
   unsigned short (*lpfnVemEnableExtension)(VEM_KEY extensionKey);
   unsigned short (*lpfnVemAttachExtension)(VEM_KEY stateKey, VEM_KEY extensionKey, 
      unsigned short priority);
   unsigned short (*lpfnVemDetachExtension)(VEM_KEY stateKey, VEM_KEY extensionKey);
   VEM_FUNC *(*lpfnVemGetExtensionFunc)(const VEM_KEY extensionKey);
   unsigned short (*lpfnVemSetExtensionFunc)(VEM_KEY extensionKey, VEM_FUNC *vemFunc);
   void *(*lpfnVemGetExtensionData)(const VEM_KEY extensionKey);
   unsigned short (*lpfnVemSetExtensionData)(VEM_KEY extensionKey, void *vemFuncMem);
   void (*lpfnVemShowExtensions)(const void *hVemMemHandle);

   /******* Parameters *******/
   VEM_KEY (*lpfnVemUnregisterParameter)(void *hVemMemHandle, VEM_KEY pvVemParam);
   VEM_KEY (*lpfnVemRegisterArray)(void *hVemMemHandle, void *pData, PARAMETER_TYPE dataType, 
                            size_t dataSize, const char *pszKey);
   VEM_KEY (*lpfnVemRegisterTable)(void *hVemMemHandle, void *pData, PARAMETER_TYPE dataType, 
                            size_t dataSize, const char *pszKey);
   VEM_KEY (*lpfnVemRegisterVariable)(void *hVemMemHandle, void *pData, 
                               PARAMETER_TYPE paramType, const char *pszKey);
   VEM_KEY (*lpfnVemRegisterStruct)(void *hVemMemHandle, void *pData, size_t dataSize, 
                             const char *pszKey);
   VEM_KEY (*lpfnVemRegisterConstFloat)(void *pvMemMain, float val, const char *pszKey);
   VEM_KEY (*lpfnVemRegisterConstShort)(void *pvMemMain, short val, const char *pszKey);
   VEM_KEY (*lpfnVemGetParameter)(void *hVemMemHandle, const char *pszKey);
   unsigned short (*lpfnVemGetParameterData)(VEM_KEY pvVemParameter, void *pDataPtr);
   unsigned short (*lpfnVemSetParameterData)(VEM_KEY pvVemParameter, void *pDataPtr);
   void (*lpfnVemShowParameters)(const void *hVemMemHandle);
} VEM_METHOD_STRUCT;


/***************************************************
* public structure: VEM2
****************************************************/
typedef struct tagVEM2 {
   void *pVemStates;
   void *pVemExtensions;
   void *pVemParameters;
   void *pFunctions;
} VEM2;


/***************************************************
* public structure: VEM_MAIN_STRUCT
****************************************************/
typedef struct tagVEM_MAIN_STRUCT {
   VEM2              *pMemoryHandler;
   VEM_METHOD_STRUCT *pVemMethodStruct;
} VEM_MAIN_STRUCT;

#if(VEM_DEFINED==1)

/******* States *******/

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
*******************************************************************************/
VEM_KEY vemUnregisterState(const void *hVemMemHandle, VEM_KEY stateKey);

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
*******************************************************************************/
VEM_KEY vemRegisterState(const void *hVemMemHandle, const char *pszKey);

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
*******************************************************************************/
unsigned short vemServiceState(VEM_KEY stateKey);

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
*******************************************************************************/
unsigned short vemDisableState(VEM_KEY stateKey);

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
*******************************************************************************/
unsigned short vemEnableState(VEM_KEY  stateKey);

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
*******************************************************************************/
VEM_KEY vemGetState(const void *hVemMemHandle, const char *pszKey);


/******************************************************************************
*
* Function:  vemShowStates()
*
* Action:    Print out a list of all states currently registered under the current
*            Vem memory handle. Every state that has one or more extensions attached
*            will also be listed.  If VEM_TRACE == 0, this is a NULL function.
*
* Input:    hVemMemHandle -- main Vem memory handle
*
* Output:   print the messages on stdout
*
* Globals:  none
*
* Return:   none
*******************************************************************************/
void vemShowStates(const void *hVemMemHandle);

/******* Extensions *******/

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
*******************************************************************************/
VEM_KEY vemUnregisterExtension(const void *hVemMemHandle, VEM_KEY extensionKey);

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
*******************************************************************************/
VEM_KEY vemRegisterExtension(const void *hVemMemHandle, const char *pszKey, 
                             VEM_FUNC *pVemFunc, VEM_FUNC *pVemFuncFree, 
                             void *pVemFuncMem);


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
*******************************************************************************/
VEM_KEY vemGetExtension(const void *hVemMemHandle, const char *pszKey);

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
*******************************************************************************/
unsigned short vemDisableExtension(VEM_KEY extensionKey);

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
*******************************************************************************/
unsigned short vemEnableExtension(VEM_KEY extensionKey);

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
*******************************************************************************/
unsigned short vemAttachExtension(VEM_KEY stateKey, VEM_KEY extensionKey, 
                                  const unsigned short priority);

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
*******************************************************************************/
unsigned short vemDetachExtension(VEM_KEY stateKey, VEM_KEY extensionKey);

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
*******************************************************************************/
VEM_FUNC *vemGetExtensionFunc(const VEM_KEY extensionKey);


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
*******************************************************************************/
unsigned short vemSetExtensionFunc(VEM_KEY extensionKey, VEM_FUNC *vemFunc);

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
*******************************************************************************/
void *vemGetExtensionData(const VEM_KEY extensionKey);

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
*******************************************************************************/
unsigned short vemSetExtensionData(VEM_KEY extensionKey, void *vemFuncMem);


/******************************************************************************
*
* Function:  vemShowExtensions()
*
* Action:   If VEM_TRACE is 1, then show all extensions and every extension's 
*           states attachment. Otherwise, it's a null function.
*
* Input:    hVemMemHandle -- main Vem memory handle
*
* Output:   print the messages on stdout
*
* Globals:  none
*
* Return:   none
*******************************************************************************/
void vemShowExtensions(const void *hVemMemHandle);

/* main create and destroy routines */

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
*******************************************************************************/
void *vemFree(void *hVemMainHandle);


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
*******************************************************************************/
void *vemInit(void);

#else /* VEM_DEFINED */

/******* States *******/
#define vemUnregisterState(hVemMemHandle, stateKey)     ((VEM_KEY)NULL)
#define vemRegisterState(hVemMemHandle, pszKey)         ((VEM_KEY)NULL)
#define vemServiceState(stateKey)                       ((VEM_KEY)NULL)
#define vemDisableState(stateKey)                       ((VEM_KEY)NULL)
#define vemEnableState(stateKey)                        ((VEM_KEY)NULL)
#define vemGetState(hVemMemHandle, pszKey)              ((VEM_KEY)NULL)

#define vemShowStates(hVemMemHandle)                    ((VEM_KEY)NULL)

/******* Extensions *******/
#define vemUnregisterExtension(hVemMemHandle, extensionKey) ((VEM_KEY)NULL)
#define vemRegisterExtension(hVemMemHandle, pszKey, pVemFunc, \
   pVemFuncFree, pVemFuncMem)                           ((VEM_KEY)NULL)
#define vemGetExtension(hVemMemHandle, pszKey)          ((VEM_KEY)NULL)
#define vemDisableExtension(extensionKey)               ((VEM_KEY)NULL)
#define vemEnableExtension(extensionKey)                ((VEM_KEY)NULL)
#define vemAttachExtension(stateKey, extensionKey, priority) ((VEM_KEY)NULL)
#define vemDetachExtension(stateKey, extensionKey)      ((VEM_KEY)NULL)

#define vemGetExtensionFunc(extensionKey)               ((VEM_KEY)NULL)
#define vemSetExtensionFunc(extensionKey, vemFunc)      ((VEM_KEY)NULL)
#define vemGetExtensionData(extensionKey)               ((VEM_KEY)NULL)
#define vemSetExtensionData(extensionKey, vemFuncMem)   ((VEM_KEY)NULL)

#define vemShowExtensions(hVemMemHandle)                ((VEM_KEY)NULL)

/* main create and destroy routines */
#define vemFree(hVemMainHandle)                         ((VEM_KEY)NULL)
#define vemInit()                                       ((VEM_KEY)NULL)

#endif /* VEM_DEFINED */

#endif /* __VEM2 */



