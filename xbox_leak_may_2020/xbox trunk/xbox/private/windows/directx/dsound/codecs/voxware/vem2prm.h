/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:        vem2Prm.h
*
* Purpose:  main vem file needed to locate codec parameters
*
* Functions:   vemUnRegisterParameter, ...
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
* $Header:   /export/phobos/pvcs/r_and_d/archives/common/vem/vem2Prm.h_v   1.3   25 Mar 1998 14:23:16   weiwang  $
******************************************************************************/

#ifndef __VEM2PRM
#define __VEM2PRM

/* switch for DSP's */
#define VEM_DEFINED 1

#ifndef __VEM_KEY
#define __VEM_KEY
typedef void * VEM_KEY;
#endif /* __VEM_KEY */

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

#if(VEM_DEFINED==1)

/******************************************************************************
*
* Function:  vemUnRegisterParameter()
*
* Action:   Remove a parameter from the Vem main memory handle.
*
* Input:    hVemMemHandle -- main Vem memory handle
*           paramKey -- key to locate paramter
*
* Output:   hVemMemHandle -- updated Vem memory handle
*
* Globals:  none
*
* Return:   NULL key
*******************************************************************************/
VEM_KEY vemUnregisterParameter(void *hVemMemHandle, VEM_KEY pvVemParam);

/******************************************************************************
*
* Function:  vemRegisterArray()
*
* Action:   register arrays. Note: no constant arrays can be registered with 
*           this function.
*
* Input:    hVemMemHandle -- main Vem memory handle
*           pData -- pointer to array element
*           dataType -- parameter type
*           dataSize -- size of array
*           pszKey -- unique parameter identifier string
*
* Output:   none
*
* Globals:  none
*
* Return:   key to locate parameter
*******************************************************************************/
VEM_KEY vemRegisterArray(void *hVemMemHandle, void *pData, PARAMETER_TYPE dataType, 
                         size_t dataSize, const char *pszKey);

/******************************************************************************
*
* Function:  vemRegisterTable()
*
* Action:   register tables.
*
* Input:    hVemMemHandle -- main Vem memory handle
*           pData -- pointer to table element
*           dataType -- parameter type
*           dataSize -- size of table
*           pszKey -- unique parameter identifier string
*
* Output:   none
*
* Globals:  none
*
* Return:   key to locate parameter
*******************************************************************************/
VEM_KEY vemRegisterTable(void *hVemMemHandle, void *pData, PARAMETER_TYPE dataType, 
                         size_t dataSize, const char *pszKey);

/******************************************************************************
*
* Function:  vemRegisterVariable()
*
* Action:   register variables
*
* Input:    hVemMemHandle -- main Vem memory handle
*           pData -- pointer to variable element
*           dataType -- parameter type
*           pszKey -- unique parameter identifier string
*
* Output:   none
*
* Globals:  none
*
* Return:   key to locate parameter
*******************************************************************************/
VEM_KEY vemRegisterVariable(void *hVemMemHandle, void *pData, 
                            PARAMETER_TYPE paramType, const char *pszKey);

/******************************************************************************
*
* Function:  vemRegisterStruct()
*
* Action:   register structure
*
* Input:    hVemMemHandle -- main Vem memory handle
*           pData -- pointer to structure
*           dataSize -- structure size
*           pszKey -- unique parameter identifier string
*
* Output:   none
*
* Globals:  none
*
* Return:   key to locate parameter
*******************************************************************************/
VEM_KEY vemRegisterStruct(void *hVemMemHandle, void *pData, size_t dataSize, 
                          const char *pszKey);

/******************************************************************************
*
* Function:  vemRegisterConstFloat()
*
* Action:   register constants
*
* Input:    pvMemMain -- main Vem memory handle
*           val -- constant float value
*           pszKey -- unique parameter identifier string
*
* Output:   none
*
* Globals:  none
*
* Return:   key to locate parameter
*******************************************************************************/
VEM_KEY vemRegisterConstFloat(void *pvMemMain, float val, const char *pszKey);


/******************************************************************************
*
* Function:  vemRegisterConstShort()
*
* Action:   register constants
*
* Input:    pvMemMain -- main Vem memory handle
*           val -- constant short value
*           pszKey -- unique parameter identifier string
*
* Output:   none
*
* Globals:  none
*
* Return:   key to locate parameter
*******************************************************************************/
VEM_KEY vemRegisterConstShort(void *pvMemMain, short val, const char *pszKey);


/******************************************************************************
*
* Function:  vemGetParameter()
*
* Action:   This function returns VEM_KEY that the function was registered with.
*
* Input:    hVemMemHandle -- main Vem memory handle
*           pszKey -- unique parameter identifier string
*
* Output:   none
*
* Globals:  none
*
* Return:   key to locate parameter
*******************************************************************************/
VEM_KEY vemGetParameter(void *hVemMemHandle, const char *pszKey);

/******************************************************************************
*
* Function:  vemGetParameterData()
*
* Action:   This function returns a COPY of the data.
*
* Input:    pvVemParameter -- key to locate paramter
*           pData -- address to location to return copy of data
*
* Output:   none
*
* Globals:  none
*
* Return:   0: succeed, 1: failed
*******************************************************************************/
unsigned short vemGetParameterData(VEM_KEY pvVemParameter, void *pDataPtr);

/******************************************************************************
*
* Function:  vemSetParameterData()
*
* Action:   This function copies user provided contents to the Vem 
*
* Input:    pvVemParamter -- key to locate paramter
*           pData -- address of location where data is copied from
*
* Output:   none
*
* Globals:  none
*
* Return:   0: succeed, 1: failed
*******************************************************************************/
unsigned short vemSetParameterData(VEM_KEY pvVemParameter, void *pDataPtr);



/******************************************************************************
*
* Function:  vemShowParameters()
*
* Action:   This diagnostic utility will give a complete list of all parameters 
*           that are registered under the main Vem memory handle.
*
* Input:    hVemMemHandle -- main Vem memory handle
*
* Output:   diagnostic messages on Stdout.
*
* Globals:  none
*
* Return:   none
*******************************************************************************/
void vemShowParameters(const void *hVemMemHandle);

#else /* VEM_DEFINED */
#define vemUnregisterParameter(hVemMemHandle, pvVemParam)      ((VEM_KEY)NULL)

#define vemRegisterArray(hVemMemHandle, pData, dataType, \
                         dataSize, pszKey)                     ((VEM_KEY)NULL)

#define vemRegisterTable(hVemMemHandle, pData, dataType,  \
                         dataSize, pszKey)                     ((VEM_KEY)NULL)

#define vemRegisterVariable(hVemMemHandle, pData, \
                            paramType, pszKey)                 ((VEM_KEY)NULL)

#define vemRegisterStruct(hVemMemHandle, pData, dataSize, pszKey) ((VEM_KEY)NULL)

#define vemRegisterConstFloat(pvMemMain, val, pszKey)         ((VEM_KEY)NULL)

#define vemRegisterConstShort(pvMemMain, val, pszKey)         ((VEM_KEY)NULL)


#define vemGetParameter(hVemMemHandle, pszKey)                ((VEM_KEY)NULL)
#define vemGetParameterData(pvVemParameter, pDataPtr)         ((VEM_KEY)NULL)
#define vemSetParameterData(pvVemParameter, pDataPtr)         ((VEM_KEY)NULL)
#define vemShowParameters(hVemMemHandle)                      ((VEM_KEY)NULL)
#endif /* VEM_DEFINED */

#endif /* __VEM2PRM */






