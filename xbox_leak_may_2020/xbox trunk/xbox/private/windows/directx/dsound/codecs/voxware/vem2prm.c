/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:        vem2Prm.c
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
* $Header:   /export/phobos/pvcs/r_and_d/archives/common/vem/vem2Prm.c_v   1.4   25 Mar 1998 14:23:16   weiwang  $
******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "VoxMem.h"
#include "vector.h"
#include "vem2.h"

#include "vem2Prm.h"

#include "xvocver.h"

#if VEM_DEFINED == 1
/*--------------------------------------------------
  Defines the private structure: ELEMENT_TYPE
  --------------------------------------------------*/
typedef enum tagELEMENT_TYPE {
   VEM_VARIABLE,
   VEM_FLOAT_CONST,
   VEM_SHORT_CONST,
   VEM_ARRAY,
   VEM_TABLE,
   VEM_STRUCT
} ELEMENT_TYPE;


/*--------------------------------------------------
  Defines the private structure: VEM_PARAMETER
  --------------------------------------------------*/
typedef struct tagVEM_PARAMETER {
   char           *pszKey;
   union tagDataContainer {
      void        *pData;
      float       floatData;
      short       shortData;
   } DataContainer;
   size_t         dataSize;
   PARAMETER_TYPE dataType;
   ELEMENT_TYPE   elementType;
} VEM_PARAMETER;


/*------------------------------------------------------------
  prototypes of private functions.
  ------------------------------------------------------------*/
static unsigned short copyParameter(void *pVector, const void *pVector2, 
                         PARAMETER_TYPE paramType, size_t count);

static VEM_KEY registerParameter(void *hVemMemHandle, VEM_PARAMETER **hVemParameter, const char *pszKey);






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
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:  Parameters cann't be disabled in the same fashion as states and 
*            externsions, so this is the only way to deny access to other
*            code segments.
*
* Concerns/TBD:
******************************************************************************/

VEM_KEY vemUnregisterParameter(void *hVemMemHandle, VEM_KEY paramKey)
{
   VEM2              *pVemMemHandler   = (VEM2 *)hVemMemHandle;

   VEM_PARAMETER     *pVemParameter    = (VEM_PARAMETER *)vecGetData((void*)paramKey);

   if(pVemParameter) {
      vecDelElement(pVemMemHandler->pVemParameters, paramKey);
      VOX_MEM_FREE(pVemParameter->pszKey);
      VOX_MEM_FREE(pVemParameter);
   }
   return ((VEM_KEY)NULL);
}


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

VEM_KEY vemRegisterArray(void *hVemMemHandle, void *pData, PARAMETER_TYPE dataType, 
                         size_t dataSize, const char *pszKey)
{
   VEM_KEY paramKey=NULL;
   VEM_PARAMETER *pVemParameter=NULL;

   if(!(paramKey=registerParameter(hVemMemHandle,&pVemParameter, pszKey)))
      return ((VEM_KEY)NULL);

   pVemParameter->DataContainer.pData=pData;
   pVemParameter->dataSize=dataSize;
   pVemParameter->dataType=dataType;
   pVemParameter->elementType=VEM_ARRAY;

   return(paramKey);
}

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

VEM_KEY vemRegisterTable(void *hVemMemHandle, void *pData, PARAMETER_TYPE dataType, 
                         size_t dataSize, const char *pszKey)
{
   VEM_KEY            paramKey         = NULL;
   VEM_PARAMETER     *pVemParameter    = NULL;

   if(!(paramKey=registerParameter(hVemMemHandle,&pVemParameter, pszKey)))
      return ((VEM_KEY)NULL);

   pVemParameter->DataContainer.pData=pData;
   pVemParameter->dataSize=dataSize;
   pVemParameter->dataType=dataType;
   pVemParameter->elementType=VEM_TABLE;

   return(paramKey);
}


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

VEM_KEY vemRegisterVariable(void *hVemMemHandle, void *pData, 
                            PARAMETER_TYPE dataType, const char *pszKey)
{
   VEM_KEY            paramKey         = NULL;
   VEM_PARAMETER     *pVemParameter    = NULL;

   if(!(paramKey=registerParameter(hVemMemHandle,&pVemParameter, pszKey)))
      return ((VEM_KEY)NULL);

   pVemParameter->DataContainer.pData=pData;
   pVemParameter->dataSize=0;
   pVemParameter->dataType=dataType;
   pVemParameter->elementType=VEM_VARIABLE;

   return(paramKey);
}


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

VEM_KEY vemRegisterStruct(void *hVemMemHandle, void *pData, size_t dataSize, 
                          const char *pszKey)
{
   VEM_KEY            paramKey         = NULL;
   VEM_PARAMETER     *pVemParameter    = NULL;

   if(!(paramKey=registerParameter(hVemMemHandle,&pVemParameter, pszKey)))
      return ((VEM_KEY)NULL);

   pVemParameter->DataContainer.pData=pData;
   pVemParameter->dataSize=dataSize;
   pVemParameter->dataType=VEM_VOID; /* type doesn't matter */
   pVemParameter->elementType=VEM_STRUCT;

   return(paramKey);
}


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

VEM_KEY vemRegisterConstFloat(void *pvMemMain, float val, const char *pszKey) {
   VEM_KEY            paramKey         = NULL;
   VEM_PARAMETER     *pVemParameter    = NULL;

   if(!(paramKey=registerParameter(pvMemMain,&pVemParameter, pszKey)))
      return ((VEM_KEY)NULL);

   pVemParameter->DataContainer.floatData=val;
   pVemParameter->dataSize=0;
   pVemParameter->dataType=VEM_FLOAT;
   pVemParameter->elementType=VEM_FLOAT_CONST;

   return(paramKey);
}

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

VEM_KEY vemRegisterConstShort(void *pvMemMain, short val, const char *pszKey)
{
   VEM_KEY            paramKey         = NULL;
   VEM_PARAMETER     *pVemParameter    = NULL;

   if(!(paramKey=registerParameter(pvMemMain,&pVemParameter, pszKey)))
      return ((VEM_KEY)NULL);

   pVemParameter->DataContainer.shortData=val;
   pVemParameter->dataSize=0;
   pVemParameter->dataType=VEM_SHORT;
   pVemParameter->elementType=VEM_SHORT_CONST;

   return(paramKey);
}


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

VEM_KEY vemGetParameter(void *hVemMemHandle, const char *pszKey)
{
   VEM2              *pVemMemHandler   = (VEM2 *)hVemMemHandle;

   void              *pElement         = vecGetFirstElement(pVemMemHandler->pVemParameters);

   while(pElement) {
      if(!strcmp(((VEM_PARAMETER *)vecGetData(pElement))->pszKey,pszKey))
         break;
      pElement=vecGetNextElement(pElement, FORWARDS);
   }
   return((VEM_KEY)pElement); /* could be NULL */
}

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

unsigned short vemGetParameterData(VEM_KEY pvVemParameter, void *pData)
{
   VEM_PARAMETER *pVemParameter=(VEM_PARAMETER *)vecGetData((void *)pvVemParameter);

   if(pVemParameter) {
      switch(pVemParameter->elementType) {
         case VEM_ARRAY:
         case VEM_TABLE:
            return(copyParameter(pData,pVemParameter->DataContainer.pData,pVemParameter->dataType,
                   pVemParameter->dataSize));
         case VEM_VARIABLE:
            return(copyParameter(pData,pVemParameter->DataContainer.pData,pVemParameter->dataType,1));
         case VEM_FLOAT_CONST:
            *(float *)pData=pVemParameter->DataContainer.floatData;
            return 0;
         case VEM_SHORT_CONST:
            *(short *)pData=pVemParameter->DataContainer.shortData;
            return 0;
         case VEM_STRUCT:
            return((unsigned short)((memcpy(pData,pVemParameter->DataContainer.pData,
               pVemParameter->dataSize))?(unsigned short)0:(unsigned short)1));
         default:
            assert(0);
      }
   } else
      return 1;
      
   return 0;   
}


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

unsigned short vemSetParameterData(VEM_KEY pvVemParameter, void *pData)
{
   VEM_PARAMETER *pVemParameter=(VEM_PARAMETER *)vecGetData((void *)pvVemParameter);

   if(pVemParameter) {
      switch(pVemParameter->elementType) {
         case VEM_ARRAY:
            return(copyParameter(pVemParameter->DataContainer.pData,pData,pVemParameter->dataType,
                   pVemParameter->dataSize));
         case VEM_VARIABLE:
            return(copyParameter(pVemParameter->DataContainer.pData,pData,pVemParameter->dataType,1));
         case VEM_TABLE:
         case VEM_FLOAT_CONST:
         case VEM_SHORT_CONST:
            return 1;
         case VEM_STRUCT:
            return((unsigned short)((memcpy(pVemParameter->DataContainer.pData,pData,
                   pVemParameter->dataSize))?0:1));
         default:
            assert(0);
      }
   } else
      return 1;
      
   return 0;   
}




/******************************************************************************
*
* Function:  registerParameter()
*
* Action:   parameter registration
*
* Input:    hVemMemHandle -- main Vem memory handle
*           hVemParameter -- pointer to Vem parameter
*           pszKey -- unique parameter identifier string
*
* Output:   none
*
* Globals:  none
*
* Return:   key to locate parameter
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:  local function
*
* Concerns/TBD:
******************************************************************************/
static VEM_KEY registerParameter(void *hVemMemHandle, VEM_PARAMETER **hVemParameter, 
                                 const char *pszKey)
{
   VEM2              *pVemMemHandler   = (VEM2 *)hVemMemHandle;
   VEM_PARAMETER     *pVemParameter    = NULL;
   VEM_KEY            paramKey         = NULL;

   if(VOX_MEM_INIT(pVemParameter=*hVemParameter, 1, sizeof(VEM_PARAMETER)))
      return NULL;
   else
      pVemParameter->pszKey=NULL;

   if(!(paramKey=vecAddElement(pVemMemHandler->pVemParameters, pVemParameter, FORWARDS))) {
      VOX_MEM_FREE(pVemParameter);
      return NULL;
   }

   if(VOX_MEM_INIT(pVemParameter->pszKey,strlen(pszKey)+1,sizeof(char)))
      return vemUnregisterParameter(hVemMemHandle, paramKey);
   else
      strcpy(pVemParameter->pszKey,pszKey);

   return paramKey;
}



/******************************************************************************
*
* Function:  copyParameter()
*
* Action:   copy parameter
*
* Input:    pVector -- array where parameters copy to
*           pVector2 -- array where parameter copy from
*           paramType -- the parameter type
*           count -- size of the parameters
*
* Output:   pVector -- array with new parameters
*
* Globals:  none
*
* Return:   0: succeed, 1: failed
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:  local function
*
* Concerns/TBD:
******************************************************************************/

static unsigned short copyParameter(void *pVector, const void *pVector2, 
                         PARAMETER_TYPE paramType, size_t count) 
{
   size_t i;

   unsigned char  *pUChar=NULL,    *pUChar2=NULL;
   unsigned short *pUShort=NULL,   *pUShort2=NULL;
   unsigned int   *pUInt=NULL,     *pUInt2=NULL;
   unsigned long  *pULong=NULL,    *pULong2=NULL;
   float          *pFloat=NULL,    *pFloat2=NULL;
   double         *pDouble=NULL,   *pDouble2=NULL;

   switch(paramType) {
      case VEM_CHAR: 
      case VEM_U_CHAR:
         pUChar=(unsigned char *)pVector;
         pUChar2=(unsigned char *)pVector2;
         for(i=0;i<count;i++)
            pUChar[i]=pUChar2[i];
         break;
      case VEM_SHORT:
      case VEM_U_SHORT:
         pUShort=(unsigned short *)pVector;
         pUShort2=(unsigned short *)pVector2;
         for(i=0;i<count;i++)
            pUShort[i]=pUShort2[i];
         break;
      case  VEM_INT:
      case  VEM_U_INT:
         pUInt=(unsigned int *)pVector;
         pUInt2=(unsigned int *)pVector2;
         for(i=0;i<count;i++)
            pUInt[i]=pUInt2[i];
         break;
      case VEM_LONG:
      case VEM_U_LONG:
         pULong=(unsigned long *)pVector;
         pULong2=(unsigned long *)pVector2;
         for(i=0;i<count;i++)
            pULong[i]=pULong2[i];
         break;
      case VEM_FLOAT:
         pFloat=(float *)pVector;
         pFloat2=(float *)pVector2;
         for(i=0;i<count;i++)
            pFloat[i]=pFloat2[i];
         break;
      case VEM_DOUBLE:
         pDouble=(double *)pVector;
         pDouble2=(double *)pVector2;
         for(i=0;i<count;i++)
            pDouble[i]=pDouble2[i];
         break;
      default:
         return 1;
   }
   return 0;
}

#if VEM_TRACE==1

#include <stdio.h>



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

void vemShowParameters(const void *hVemMemHandle)
{
   VEM_PARAMETER     *pVemParameter    = NULL;
   void              *pVemElement      = NULL, *pVemElement2 = NULL;
   VEM2              *pVemMemHandler   = (VEM2 *)hVemMemHandle;

   pVemElement = vecGetFirstElement(pVemMemHandler->pVemParameters);
   if(pVemElement)
      printf("\n\nParameters registered: ");
   else
      printf("\nNo parameters registered ");
   while(pVemElement) {
      pVemParameter=(VEM_PARAMETER *)vecGetData(pVemElement);
      printf("\n   \"%10s\" : ", pVemParameter->pszKey);
      switch(pVemParameter->elementType) {
         case VEM_FLOAT_CONST:
         case VEM_SHORT_CONST:
            printf("const ");
            break;
         case VEM_ARRAY:
            printf("array[%d] of ", pVemParameter->dataSize);
            break;
         case VEM_TABLE:
            printf("const array[%d] of ", pVemParameter->dataSize);
            break;
         case VEM_VARIABLE:
            break;
      }
      switch(pVemParameter->dataType) {
         case VEM_CHAR:
            printf("char");
            break;
         case VEM_U_CHAR:
            printf("unsigned char");
            break;
         case VEM_SHORT:
            printf("short");
            break;
         case VEM_U_SHORT:
            printf("unsigned short");
            break;
         case VEM_INT:
            printf("int");
            break;
         case VEM_U_INT:
            printf("unsigned int");
            break;
         case VEM_LONG:
            printf("long");
            break;
         case VEM_U_LONG:
            printf("unsigned long");
            break;
         case VEM_FLOAT:
            printf("float");
            break;
         case VEM_DOUBLE:
            printf("double");
            break;
      }
      pVemElement=vecGetNextElement(pVemElement, FORWARDS);
   }
   return;   
}

#else /* VEM_TRACE */
/* if VEM_TRACE is off, there will be warnings for unreferenced paramenters. 
   Please ignore them */
void vemShowParameters(const void *hVemMemHandle) {return;}
#endif /* VEM_TRACE */
#endif  /* VEM_DEFINED */

