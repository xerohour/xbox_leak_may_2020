//*****************************Module*Header******************************
//
// Module Name: wkselsatoolsregistry.c
//
// FNicklisch 09/14/2000: New, derived from registry.c
//
// This part of code was taken from the code bought from ELSA. Parts of 
// it is used to get the workstation tools, application and features up
// and running. It is implemented on a temporary base and will be replaced
// by a NVIDIA propritary interface as soon as possible.
// Don't care about the code not necessariliy used.
//
// Copyright (c) 1999-2000, ELSA AG, Aachen (Germany)
// Copyright (c) 2000 NVidia Corporation. All Rights Reserved.
//

// import header files

#include "precomp.h"
#ifdef USE_WKS_ELSA_TOOLS
#include "wkselsatoolsdebug.h"
#include "wkselsatoolsescape.h"
#include "wkselsatoolstoolbox.h"
#include "ELSAESC.h"

// export header file
#include <wkselsatoolsregistry.h>



/*
** Registry.c
**
** Copyright (c) 1999, ELSA AG, Aachen (Germany)
**
** Internal Registry interface 
**
*/


// Code from ELSAESC.h that is in the driver but isn't used and reported yet
#define SUBESC_ET_SET_VALUE_DEFAULT         0x2111111A // set default (any type)
// In structure (Tool -> driver):
// Input Structure to set a registry default for either DWORD or a STRING
// Output Structure ET_SET_VALUE_RET
typedef struct tagET_SET_VALUE_DEFAULT
{
  ET_QUERY hdr;     // size and subescape SUBESC_ET_SET_VALUE_DEFAULT

  DWORD    dwID;    // ID of value to set (ET_VALUE_...)
  DWORD    dwType;  // must be ET_VALUE_TYPE_DWORD, ET_VALUE_TYPE_STRING, ...

} ET_SET_VALUE_DEFAULT, *PET_SET_VALUE_DEFAULT;

static BOOL bRegQueryAgainstInternalStatus(
      PPDEV           ppdev, 
  IN  DWORD           dwSubEsc, 
  IN  ET_QUERY_VALUE *petQueryValue,
  IN  ULONG           cjOut, 
  OUT VOID           *pvOut,
  OUT LONG           *plRet);

/*
** lEscRegistryValue
**
** escape wrapper for all registry interface calls.
** It checks in/out interface structure sizes,
** and dispatches the sub escapes.
**
** return: ESC_SUCCESS        function succeded without error            
**         ESC_NOTSUPPORTED   function not supported                     
**         ESC_ERROR          function supported but error while working 
**
** FNicklis 08.12.98: New
*/

LONG lEscRegistryValue(
  PPDEV    ppdev,                                               
  ULONG    dwSubEsc, 
  ULONG    cjIn ,
  VOID    *pvIn ,
  ULONG    cjOut,
  VOID    *pvOut)
{
  LONG   lRet   = ESC_ERROR; // Default error 
  HANDLE hDriver;

  DBG_TRACE_IN(DBG_LVL_FLOW0, lEscRegistryValue);

  ASSERT(NULL!=ppdev);
  ASSERT(NULL!=pvIn);
  ASSERT(cjIn >= sizeof(ET_QUERY));

#ifdef MULTI_BOARD
  hDriver = hGetBoardhDriver(ppdev, ET_QUERY_GET_BOARD(pvIn));
#else
  hDriver = ppdev->hDriver;
#endif

  switch (dwSubEsc)
  {
    case SUBESC_ET_QUERY_VALUE_STRING:  
    case SUBESC_ET_QUERY_VALUE_DWORD :   
    case SUBESC_ET_QUERY_VALUE_BINARY:
    {
      if (cjIn < sizeof(ET_QUERY_VALUE))
      {
        DBG_ERROR("cjIn < sizeof(ET_QUERY_VALUE)");
        break;
      }

      if (!bRegQueryAgainstInternalStatus(ppdev, dwSubEsc, (ET_QUERY_VALUE *)pvIn, cjOut, pvOut, &lRet))
      {
        // call not handled internaly, go through registry
        lRet = lRegEtQueryValue(hDriver, dwSubEsc, (ET_QUERY_VALUE *)pvIn, cjOut, pvOut);
      }

      break;
    }

    case SUBESC_ET_SET_VALUE_STRING:
    case SUBESC_ET_SET_VALUE_DWORD :
    case SUBESC_ET_SET_VALUE_BINARY:
    {
      if ( (NULL==pvOut) ||
           (sizeof(ET_SET_VALUE_RET) > cjOut) )
      {
        DBG_ERROR("SUBESC_ET_SET_VALUE_STRING/DWORD cjOut to small");
        break;
      }

      lRet = lRegEtSetValue(hDriver, dwSubEsc, pvIn, cjIn, (ET_SET_VALUE_RET *)pvOut);
      break;
    }

    default:
    {
      ASSERTDD(dwSubEsc==SUBESC_ET_QUERY_VALUE_STRING,"unhandled and not supported sub escape");
      lRet = ESC_NOTSUPPORTED;
      break;
    }
  }

  DBG_TRACE_OUT(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED, lRet);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX, " (%s)",szDbg_GetESC_RETURN(lRet) ));
  return (lRet);
}


// *******************************************************************************************
// * Begin of functions manipulationg registry via ID code
// *******************************************************************************************

/*
** lRegEtQueryValue 
**
** Esc interface to retrieve any driver handled registry key
**
** return : ESC_SUCCESS      ( 1) : function finished successfully
**          ESC_NOTSUPPORTED ( 0) : ERROR: iEsc or SubEsc not supported
**          ESC_ERROR        (-1) : ERROR: any other error (wrong feeding)
**
** 141098 MSchwarz: New
** FNicklis 21.10.98: fixed interface mischmasch
*/
LONG lRegEtQueryValue(
  IN  HANDLE          hDriver, 
  IN  DWORD           dwSubEsc, 
  IN  ET_QUERY_VALUE *petQueryValue,
  IN  ULONG           cjOut, 
  OUT VOID           *pvOut)
{
  LONG  lRet      = ESC_ERROR;
  LONG  lResult   = REG_ERROR;
  DWORD dwID;     // recalulated and modified Key ID
  DWORD dwType;   // recalulated and modified type identifier

  DBG_TRACE_IN(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED,lRegEtQueryValue);
  ASSERTDD(NULL!=petQueryValue,"");
  DBG_PRINTX(DBG_LVL_FLOW0|DBG_LVL_NOPREFIX, petQueryValue->dwID);

  ASSERT(NULL!=hDriver);

  // Save exit on user error!
  if ( NULL==pvOut )
  {
    ASSERTDD(NULL!=pvOut, "lRegEtQueryValue:");
    goto Exit;
  }

  // check if we get the default only
  dwType = petQueryValue->dwType;
  dwID   = petQueryValue->dwID;
  if (bBitsTest(dwType,ET_VALUE_DEFAULT_FLAG))
  {
    // mask out the default type flag 
    vBitsClear(&dwType,ET_VALUE_DEFAULT_FLAG);
    // use our internal ID bit for defaults retrieval
    dwID   = INTERNAL_ET_VALUE_DEFAULT | petQueryValue->dwID;
  }

  // now decide if to get a DWORD or a String
  switch( dwSubEsc )
  {
    case SUBESC_ET_QUERY_VALUE_DWORD:
    {
      ET_VALUE_DWORD *petValueDWord = (ET_VALUE_DWORD*)pvOut;

      if (ET_VALUE_TYPE_DWORD != dwType) // must be DWord now, as Default flag is removed
      {
        DBG_ERROR("wrong type");
        break;
      }

      // Don't touch output before we have checked it!
      if (  ( sizeof(ET_VALUE_DWORD)!= cjOut )
          ||( sizeof(ET_VALUE_DWORD)!= petQueryValue->hdr.dwOutSize )  )
      {
        DBG_ERROR("outbuffer to small");
        break;
      }

      petValueDWord->dwSize   = sizeof(ET_VALUE_DWORD);
      petValueDWord->dwID     = petQueryValue->dwID;

      lResult = lRegGetIdDWord( hDriver, dwID, &petValueDWord->dwValue );

      // We handled the escape. Now we test and set for possible conditions and errors
      switch (lResult)
      {
        case REG_OK:      petValueDWord->dwFlags  = ET_VALUE_OK;
                          break;

        case REG_DEFAULT: petValueDWord->dwFlags  = ET_VALUE_DEFAULT;
                          break;

        case REG_ERROR:   petValueDWord->dwFlags  = ET_VALUE_UNKNOWN;
                          break;

        default :         RIP("lRegEtQueryValue: unknown lResult recieved from lRegGetIdxxx");
                          break;
      }

      vDbg_PrintET_VALUE_RETdwFlags(DBG_LVL_FLOW1, "petValueDWord->dwFlags",petValueDWord->dwFlags);
      lRet = ESC_SUCCESS; // Handle errors in dwFlags of return structure

      vDbg_PrintET_VALUE_DWORD(DBG_LVL_FLOW1, "petValueDWord", petValueDWord);
      break;
    }

    case SUBESC_ET_QUERY_VALUE_STRING:
    {
      ET_VALUE_STRING *petValueString = (ET_VALUE_STRING*)pvOut;

      if (ET_VALUE_TYPE_STRING != dwType) // must be String now, as Default flag is removed
      {
        DBG_ERROR("wrong type");
        break;
      }

      // Don't touch output before we have checked it!
      if (  ( sizeof(ET_VALUE_STRING)!= cjOut )
          ||( sizeof(ET_VALUE_STRING)!= petQueryValue->hdr.dwOutSize )  )
      {
        DBG_ERROR("outbuffer to small");
        break;
      }

      petValueString->dwSize   = sizeof(ET_VALUE_STRING);
      petValueString->dwID     = petQueryValue->dwID;

      lResult = lRegGetIdSz( hDriver, dwID, petValueString->szValue, ET_MAX_STRING );

      // We handled the escape. Now we test and set for possible conditions and errors
      switch (lResult)
      {
        case REG_OK:      petValueString->dwFlags  = ET_VALUE_OK;
                          break;

        case REG_DEFAULT: petValueString->dwFlags  = ET_VALUE_DEFAULT;
                          break;

        case REG_ERROR:   petValueString->dwFlags  = ET_VALUE_UNKNOWN;
                          break;

        default :         RIP("lRegEtQueryValue: unknown lResult recieved from lRegGetIdxxx");
                          break;
      }

      vDbg_PrintET_VALUE_RETdwFlags(DBG_LVL_FLOW1, "petValueString->dwFlags",petValueString->dwFlags);
      lRet = ESC_SUCCESS; // Handle errors in dwFlags of return structure

      vDbg_PrintET_VALUE_STRING(DBG_LVL_FLOW1, "petValueString", petValueString);
      break;
    }

    case SUBESC_ET_QUERY_VALUE_BINARY:
    {
      ET_VALUE_BINARY *petValueBinary = (ET_VALUE_BINARY*)pvOut;

      if (ET_VALUE_TYPE_BINARY != dwType) // must be binary now, as Default flag is removed
      {
        DBG_ERROR("wrong type");
        break;
      }

      // Don't touch output before we have checked it!
      if (  ( sizeof(ET_VALUE_BINARY) >= cjOut )
          ||( sizeof(ET_VALUE_BINARY) >= petQueryValue->hdr.dwOutSize )  )
      {
        DBG_ERROR("outbuffer to small");
        break;
      }

      petValueBinary->dwSize   = cjOut;
      petValueBinary->dwID     = petQueryValue->dwID;

      lResult = lRegGetIdBinary( hDriver, dwID, petValueBinary->ajData, cjOut-sizeof(ET_VALUE_BINARY));

      // We handled the escape. Now we test and set for possible conditions and errors
      switch (lResult)
      {
        case REG_OK:      petValueBinary->dwFlags  = ET_VALUE_OK;
                          break;

        case REG_DEFAULT: petValueBinary->dwFlags  = ET_VALUE_DEFAULT;
                          break;

        case REG_ERROR:   petValueBinary->dwFlags  = ET_VALUE_UNKNOWN;
                          break;

        default :         RIP("lRegEtQueryValue: unknown lResult recieved from lRegGetIdxxx");
                          break;
      }

      vDbg_PrintET_VALUE_RETdwFlags(DBG_LVL_FLOW1, "petValueBinary->dwFlags",petValueBinary->dwFlags);
      lRet = ESC_SUCCESS; // Handle errors in dwFlags of return structure

      break;
    }

    default:
    {
      DISPDBG((DBG_LVL_ERROR, "lRegEtQueryValue: invalid dwSubEsc:0x%x",dwSubEsc));
      lRet = ESC_NOTSUPPORTED;
      break;
    }
  }

Exit:
#ifdef DBG
  if (petQueryValue->dwType & ET_VALUE_DEFAULT_FLAG)
  {
    ASSERTDD((ESC_SUCCESS==lRet), "default was queried, why did we fail?");
    DISPDBGCOND( ((((ET_VALUE_DWORD *)pvOut)->dwFlags)!=ET_VALUE_UNKNOWN) && (ET_VALUE_DEFAULT!=((ET_VALUE_DWORD *)pvOut)->dwFlags), (DBG_LVL_ERROR, "default was queried, why don't we report this? dwFlags:0x%x", ((ET_VALUE_DWORD *)pvOut)->dwFlags));
  }
#endif
  ASSERTDD((ESC_SUCCESS==lRet)||(ESC_NOTSUPPORTED==lRet)||(ESC_ERROR==lRet), "assumed interface has changed!");

  DBG_TRACE_OUT(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED, lRet);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX, " (%s)",szDbg_GetESC_RETURN(lRet) ));

  return lRet;
}

//*******************************************************************************************
// 081098 MSchwarz 
// FNicklis 21.10.98: fixed return values
LONG lRegEtSetValue(
  IN  HANDLE            hDriver, 
  IN  DWORD             dwSubEsc, 
  IN  VOID             *pvIn, 
  IN  ULONG             cjIn, 
  OUT ET_SET_VALUE_RET *pSetValueRet)
{
  LONG     lRet      = ESC_ERROR; // Default error 
  LONG     lResult   = REG_ERROR; // Default error

  DBG_TRACE_IN(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED,lRegEtSetValue);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX,"dwID:0x%x",(pvIn) ? ((ET_SET_VALUE_DWORD*)pvIn)->dwID : -1));

  ASSERT(NULL!=hDriver);
  ASSERT(NULL!=pvIn);

  // now decide if to set a DWORD or a String
  switch( dwSubEsc )
  {
    case SUBESC_ET_SET_VALUE_DWORD:
    {
      ET_SET_VALUE_DWORD *petSetDWord = pvIn;

      if ( sizeof(ET_SET_VALUE_DWORD) > cjIn )
      {
        DBG_ERROR("sizeof(ET_SET_VALUE_DWORD) > cjIn");
        break;
      }

      pSetValueRet->dwID = petSetDWord->dwID; // return structure nees ID
      lResult = lRegSetIdDWord( hDriver, petSetDWord->dwID, 
                                petSetDWord->dwValue );

      lRet = ESC_SUCCESS; // Handle errors in dwFlags of return structure
      break;

    }
    case SUBESC_ET_SET_VALUE_STRING:
    {
      ET_SET_VALUE_STRING *petSetString = pvIn;

      if ( sizeof(ET_SET_VALUE_STRING) > cjIn )
      {
        DBG_ERROR("sizeof(ET_SET_VALUE_DWORD) > cjIn");
        break;
      }

      pSetValueRet->dwID = petSetString->dwID; // return structure nees ID
      lResult = lRegSetIdSz( hDriver, petSetString->dwID, 
                             petSetString->szValue);
  
      lRet = ESC_SUCCESS; // Handle errors in dwFlags of return structure
      break;
    }

    case SUBESC_ET_SET_VALUE_BINARY:
    {
      ET_SET_VALUE_BINARY *petSetBinary = pvIn;

      if ( sizeof(ET_SET_VALUE_BINARY) > cjIn )
      {
        DBG_ERROR("sizeof(ET_SET_VALUE_BINARY) > cjIn");
        break;
      }

      ASSERT(petSetBinary->hdr.dwSize==cjIn);
      pSetValueRet->dwID = petSetBinary->dwID; // return structure nees ID
      
      lResult = lRegSetIdBinary(hDriver, 
                                petSetBinary->dwID, 
                                (const BYTE *)petSetBinary->ajData,
                                cjIn-ET_SET_VALUE_BINARY_HDR_SIZE);
  
      lRet = ESC_SUCCESS; // Handle errors in dwFlags of return structure
      break;
    }

    default:
    {
      ASSERTDD(dwSubEsc==SUBESC_ET_SET_VALUE_DWORD, " invalid dwSubEsc");
      lRet = ESC_NOTSUPPORTED;
      break;
    }
  }

  // Handle return values and output buffer 
  // set bitfield in return structure
  // ERROR and SUCCESS should feed the output buffer error flags!
  if (ESC_NOTSUPPORTED!=lRet)
  {
    pSetValueRet->dwSize = sizeof(ET_SET_VALUE_RET);

    // We handled the escape, filled the out structure. 
    // Now we test and set for possible conditions and errors
    switch (lResult)
    {
      case REG_OK:      pSetValueRet->dwFlags  = ET_VALUE_OK;
                        break;

      case REG_DEFAULT: pSetValueRet->dwFlags  = ET_VALUE_DEFAULT;
                        break;

      case REG_ERROR:   pSetValueRet->dwFlags  = ET_VALUE_UNKNOWN;
                        break;

      default :         RIP("lRegEtSetValue: unknown lResult recieved from lRegSetIdDxx");
                        break;
    }

    vDbg_PrintET_VALUE_RETdwFlags(DBG_LVL_FLOW1, "pSetValueRet->dwFlags",pSetValueRet->dwFlags);
  }

  DBG_TRACE_OUT(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED, lRet);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX, " (%s)",szDbg_GetESC_RETURN(lRet) ));

  return lRet;
}

/*
** lRegGetIdBinary
** lRegGetIdDWord
** lRegGetIdSz
** lRegGetIdMultiSz
**
** Read nByte or a DWORD out of the registry form the key matching dwID.
** The result is stored to pjData, pulData or pstrData.
**
** NOTE: String functions work on ascii/ansi strings. WCHAR conversion is done once
** in the miniport layer.
**
** For easier code handling all functions use lRegGetIdBinary. So we have
** one path to the registry only!
**
** returns: REG_OK:      operation successful, output data valid
**          REG_DEFAULT: operation successful but default value returned, output data valid
**          REG_ERROR:   operation failed, output data not valid
**
** MSchwarz 09.10.98: New
** FNicklis 23.10.98: Fixed error handling and functionality
*/
LONG lRegGetIdBinary(
  IN  HANDLE hDriver, 
  IN  DWORD  dwID, 
  OUT BYTE  *pjData, 
  IN  DWORD  nByte )
{
  LONG                lRet                  = REG_ERROR;  // ERROR
  IOCTL_ID_TRANSFER*  piit;
  ULONG               ulLength;
  ULONG               ulReturnedDataLength  = 0;

  DBG_TRACE_IN(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED,lRegGetIdBinary);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX," dwID:%s (0x%x)",szDbg_GetET_VALUE_IDName(dwID),dwID));

  ASSERTDD(hDriver != NULL, "check environment");
  ASSERTDD(pjData  != NULL, "check caller's interface");
  ASSERTDD(nByte   != 0,    "check caller's interface");

  ulLength  =  sizeof(IOCTL_ID_TRANSFER) + nByte;

  piit = EngAllocMem( FL_ZERO_MEMORY, ulLength, ALLOC_TAG);
  //  call IOCLTRL vstatIoctlRegGetIdBinary
  //
  if( piit )
  {
    DWORD dwWIN32Error=(DWORD)-1;  // IOCTL returns Win32 error code

    piit->dwId        = dwID;
    piit->dwDataSize  = nByte;

#if 0 // FNicklisch 14.09.00: unused
        dwWIN32Error = EngDeviceIoControl( hDriver,
                                     IOCTL_VIDEO_REG_ID_TRANSFER_GET,
                                     (VOID*)(piit),sizeof(IOCTL_ID_TRANSFER),
                                     (VOID*)(piit),ulLength,
                                     &ulReturnedDataLength);
#else
        RIP("later");
#endif // FNicklisch 14.09.00: unused

    if (NO_ERROR == dwWIN32Error)
    {
      // only copy data on success or default

      ASSERTDD(ulReturnedDataLength==ulLength, "check params");

      lRet = piit->lReturnCode;                     // set error code
      RtlCopyMemory( pjData, piit->ajData, nByte ); // copy data to output buffer
    }

    EngFreeMem( piit );
  }
  else
  {
    DBG_ERROR("EngAllocMem failed");
  }

  ASSERTDD((lRet==REG_OK) || (lRet==REG_DEFAULT) || (lRet==REG_ERROR), "unknown return value!");

  DBG_TRACE_OUT(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED, lRet);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX, " (%s)",szDbg_GetREG_RETURN(lRet) ));

  return lRet;
}

/*
** lRegGetIdDWord see above
**
** FNicklis 23.10.98: New
*/
LONG lRegGetIdDWord(
  IN  HANDLE hDriver, 
  IN  DWORD  dwID, 
  OUT DWORD *pulData)
{
  LONG lRet;

  DBG_TRACE_IN(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED,lRegGetIdDWord);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX," dwID:%s (0x%x)",szDbg_GetET_VALUE_IDName(dwID),dwID));
  DBGFLAGSNEW(DBG_LVL_2WARNING);// switch of lazy redundant debug info

  lRet = lRegGetIdBinary(hDriver, dwID, (BYTE *)pulData, sizeof(DWORD));

  DBGFLAGSOLD();
  DISPDBGCOND((REG_ERROR==lRet),(DBG_LVL_WARNING, "WARNING: REG_ERROR in lRegGetIdDWord on dwID:%s (0x%x)",szDbg_GetET_VALUE_IDName(dwID),dwID));
  DISPDBG((DBG_LVL_FLOW0,"ulData:0x%x %s",( (NULL!=pulData)? (*pulData) : (-1) ), (REG_DEFAULT==lRet) ? "(default)": (REG_ERROR==lRet) ? "!!ERROR!!":""));

  DBG_TRACE_OUT(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED,lRet);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX," (%s)",(lRet==REG_OK) ? "REG_OK" : ((lRet==REG_DEFAULT) ? "REG_DEFAULT" :"REG_ERROR") ));

  return lRet;
}

/*
** lRegGetIdSz see above
**
** FNicklis 23.10.98: New
*/
LONG lRegGetIdSz(
  IN  HANDLE hDriver, 
  IN  DWORD  dwID, 
  OUT PSTR   pstrData, 
  IN  DWORD  nByte)
{
  LONG lRet;

  DBG_TRACE_IN(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED,lRegGetIdSz);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX," dwID:%s (0x%x)",szDbg_GetET_VALUE_IDName(dwID&INTERNAL_ET_VALUE_KEY_ID_ONLY_MASK),(dwID&INTERNAL_ET_VALUE_KEY_ID_ONLY_MASK)));
  DBGFLAGSNEW(DBG_LVL_2WARNING);// switch of lazy redundant debug info

  lRet = lRegGetIdBinary(hDriver, dwID, (BYTE *)pstrData, nByte);

  DBGFLAGSOLD();
  DISPDBG((DBG_LVL_FLOW0,"pstrData:%s %s",((REG_ERROR!=lRet)?pstrData:"--- invalid data ---"), (REG_DEFAULT==lRet) ? "(default)": (REG_ERROR==lRet) ? "!!ERROR!!":""));

  DBG_TRACE_OUT(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED, lRet);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX, " (%s)",szDbg_GetREG_RETURN(lRet) ));

  return lRet;
}

/*
** lRegGetIdMultiSz see above
**
** FNicklis 23.10.98: New
*/
LONG lRegGetIdMultiSz(
  IN  HANDLE hDriver, 
  IN  DWORD  dwID, 
  OUT PSTR   pstrData, 
  IN  DWORD  nByte)
{
  LONG lRet;

  DBG_TRACE_IN(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED,lRegGetIdMultiSz);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX," dwID:%s (0x%x)",szDbg_GetET_VALUE_IDName(dwID),dwID));
  DBGFLAGSNEW(DBG_LVL_2WARNING);// switch of lazy redundant debug info
  
  lRet = lRegGetIdBinary(hDriver, dwID, (BYTE *)pstrData, nByte);

  DBGFLAGSOLD();
  DISPDBG((DBG_LVL_FLOW0,"pstrData:%s %s",((REG_ERROR!=lRet)?pstrData:"--- invalid data ---"), (REG_DEFAULT==lRet) ? "(default)": (REG_ERROR==lRet) ? "!!ERROR!!":""));

  DBG_TRACE_OUT(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED, lRet);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX, " (%s)",szDbg_GetREG_RETURN(lRet) ));

  return lRet;
}

// *******************************************************************************************

/*
** lRegSetIdBinary
** lRegSetIdDWord 
** lRegSetIdSz 
** lRegSetIdMultiSz 
**
** writes nByte byte or a DWORD to the registry key matching dwID. 
** pjData and pstrData are pointers to a nByte sized buffer, 
** ulData is the value to write.
**
** NOTE: String functions work on ascii/ansi strings. WCHAR conversion is done once
** in the miniport layer.
**
** returns: REG_OK:      operation successful, data written
**          REG_ERROR:   operation failed, data not written
**         (REG_DEFAULT): a future extension could return this, if this was the default data
**
** MSchwarz 09.10.98: New
** FNicklis 23.10.98: Fixed error handling and functionality
*/
LONG lRegSetIdBinary(
  IN       HANDLE hDriver, 
  IN       DWORD  dwID, 
  IN const BYTE  *pjData, 
  IN       DWORD  nByte)
{
  LONG                lRet                 = REG_ERROR;

  IOCTL_ID_TRANSFER*  piit;
  ULONG               ulLength;
  ULONG               ulReturnedDataLength = 0;

  DBG_TRACE_IN(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED,lRegSetIdBinary);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX," dwID:0x%x (%s), pjData:0x%x, nByte:%d",dwID,szDbg_GetET_VALUE_IDName(dwID),pjData,nByte));

  ASSERTDD(hDriver != NULL, "check environment");

  ASSERTDD(pjData  != NULL, "check caller's interface");
  ASSERTDD(nByte   != 0,    "check caller's interface");

  ulLength = sizeof(IOCTL_ID_TRANSFER) + nByte;

  piit = EngAllocMem( FL_ZERO_MEMORY, ulLength, ALLOC_TAG);

  if( piit )
  {
    DWORD dwWIN32Error=(DWORD)-1;  // IOCTL returns Win32 error code

    piit->dwId        = dwID;
    piit->dwDataSize  = nByte;
    //piit->dwDataType  = REG_BINARY;
    RtlCopyMemory( piit->ajData, pjData, nByte );

#if 0 // FNicklisch 14.09.00: unused
        //  call IOCTL vstatIoctlRegSetIdBinary
    dwWIN32Error = EngDeviceIoControl( hDriver,
                                       IOCTL_VIDEO_REG_ID_TRANSFER_SET,
                                       (VOID*)(piit),ulLength,
                                       (VOID*)(piit),0,
                                       &ulReturnedDataLength);
#else
        RIP("later");
#endif // FNicklisch 14.09.00: unused
    if (NO_ERROR == dwWIN32Error)
    {
      ASSERTDD(ulReturnedDataLength==ulLength, "check params");

      lRet = piit->lReturnCode; // set error code

      if ( REG_DEFAULT==lRet )
      {
        // only default set returns default return code!
        if (!bBitsTest(dwID,INTERNAL_ET_VALUE_DEFAULT))
        {
          lRet = REG_OK;
        }
      }
    }

    EngFreeMem( piit );
  }
  else
  {
    DBG_ERROR("EngAllocMem failed");
  }

  ASSERTDD((lRet==REG_OK) || (lRet==REG_ERROR), "unknown return value!");

  DBG_TRACE_OUT(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED, lRet);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX, " (%s)",szDbg_GetREG_RETURN(lRet) ));

  return lRet;
}

/*
** lRegSetIdDWord see above
**
** FNicklis 23.10.98: New
*/
LONG lRegSetIdDWord(
  IN HANDLE hDriver, 
  IN DWORD  dwID, 
  IN DWORD  ulData)
{
  LONG lRet;

  DBG_TRACE_IN(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED,lRegSetIdDWord);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX," dwID:0x%x, ulData:0x%x",dwID,ulData));

  lRet = lRegSetIdBinary(hDriver, dwID, (const BYTE *)&ulData, sizeof(DWORD));

  DBG_TRACE_OUT(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED,lRet);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX," (%s)",(lRet==REG_OK) ? "REG_OK" : ((lRet==REG_DEFAULT) ? "REG_DEFAULT" :"REG_ERROR") ));
  return lRet;
}

/*
** lRegSetIdSz see above
**
** FNicklis 23.10.98: New
*/
LONG lRegSetIdSz(
 IN       HANDLE hDriver, 
 IN       DWORD  dwID, 
 IN const PSTR   pstrData)
{
  LONG lRet;

  DBG_TRACE_IN(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED,lRegSetIdSz);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX," dwID:0x%x, pstrData:%s",dwID,pstrData));

  lRet = lRegSetIdBinary(hDriver, dwID, (const BYTE *)pstrData, strlen(pstrData)+1/*ulStrTrueLen(pstrData)*/);

  DBG_TRACE_OUT(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED, lRet);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX, " (%s)",szDbg_GetREG_RETURN(lRet) ));

  return lRet;
}

/*
** lRegSetIdMultiSz see above
**
** FNicklis 23.10.98: New
*/
LONG lRegSetIdMultiSz(
 IN       HANDLE hDriver, 
 IN       DWORD  dwID, 
 IN const PSTR   pstrData, 
 IN       DWORD  nByte )
{
  LONG lRet;

  DBG_TRACE_IN(DBG_LVL_FLOW0,lRegSetIdMultiSz);

  lRet = lRegSetIdBinary(hDriver, dwID, (const BYTE *)pstrData, nByte);

  DBG_TRACE_OUT(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED,lRet);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX," (%s)",(lRet==REG_OK) ? "REG_OK" : ((lRet==REG_DEFAULT) ? "REG_DEFAULT" :"REG_ERROR") ));

  return lRet;
}

//*******************************************************************************************
// 081098 MSchwarz 
// FNicklis 21.10.98: fixed return values
LONG lRegEtDefaultValue(
  IN  HANDLE                hDriver, 
  IN  ET_SET_VALUE_DEFAULT *pDefaultValue, 
  OUT ET_SET_VALUE_RET     *pSetValueRet)
{
  LONG     lRet      = ESC_ERROR; // Default error 
  LONG     lResult   = REG_ERROR; // Default error

  DBG_TRACE_IN(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED,lRegEtDefaultValue);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX,"dwID:0x%x",(pDefaultValue) ? pDefaultValue->dwID : -1));

  ASSERT(NULL!=hDriver);
  ASSERT(NULL!=pDefaultValue);
  ASSERT(NULL!=pSetValueRet);

  RtlZeroMemory(pSetValueRet, sizeof(ET_SET_VALUE_RET));

  // preset return structure to error
  pSetValueRet->dwSize = sizeof(ET_SET_VALUE_RET);
  pSetValueRet->dwID   = pDefaultValue->dwID;
  pSetValueRet->dwFlags= ET_VALUE_UNKNOWN; // default error

  // To make code writing and debugging easier I implement this in
  // a two step way. First I read the default from the Miniport,
  // then I write this value again.

  // This helps me to keep the code easy, as I only need to modify
  // the miniports query function.

  // Mask out the default flag as it is useless in this call

  switch (pDefaultValue->dwType & ~ET_VALUE_DEFAULT_FLAG)
  {
    case ET_VALUE_TYPE_DWORD:
    {
      DWORD dwDefault;

      lResult = lRegGetIdDWord(
        hDriver, 
        INTERNAL_ET_VALUE_DEFAULT | pDefaultValue->dwID, 
        &dwDefault);

      if (lResult==REG_ERROR)
      {
        DBG_ERROR("lRegGetIdDWord failed");
        pSetValueRet->dwFlags= ET_VALUE_UNKNOWN;
      }
      else
      {      
        lResult = lRegSetIdDWord(
          hDriver, 
          pDefaultValue->dwID,
          dwDefault);
      }
      break;
    }

    case ET_VALUE_TYPE_STRING:
    {
      PSTR pszDefault;

      // allocate temporary buffer
      if (bAllocMem((PVOID *)&pszDefault, sizeof(ET_MAX_STRING), ALLOC_TAG))
      {
        // add default flag to read the default.
        lResult = lRegGetIdSz(
          hDriver, 
          INTERNAL_ET_VALUE_DEFAULT | pDefaultValue->dwID, 
          pszDefault,
          ET_MAX_STRING);

        if (lResult==REG_ERROR)
        {
          DBG_ERROR("lRegGetIdSz failed");
          pSetValueRet->dwFlags= ET_VALUE_UNKNOWN;
        }
        else 
        {      
          // write the default read above again!
          lResult = lRegSetIdSz(
            hDriver, 
            pDefaultValue->dwID,
            pszDefault);
        }

        // free temporary buffer again
        bFreeMem((PVOID *)&pszDefault, sizeof(ET_MAX_STRING));
      }
      break;
    }

    default:
      ASSERTDD(pDefaultValue->dwType==ET_VALUE_TYPE_STRING, "unkown ET_VALUE_TYPE_xx");
      break;
  }

  if (REG_ERROR != lResult)
  {
    pSetValueRet->dwFlags= ET_VALUE_DEFAULT;
  }
  else
  {
    DBG_ERROR("failed to set the default value");
  }

  // The escape doesn't fail! 
  // But error codes are stored in the return structure
  lRet = ESC_SUCCESS;

  DBG_TRACE_OUT(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED, lRet);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX, " (%s)",szDbg_GetESC_RETURN(lRet) ));

  return lRet;
}

// *******************************************************************************************
// * End of functions manipulationg registry via ID code
// *******************************************************************************************

#if 0 // FNicklis seems to be unused code...

//*******************************************************************************************
// 091098 MSchwarz 
LONG lRegGetValueBinary( HANDLE hDriver, PSTR pstrValue, BYTE* pjData, DWORD nByte )
{
  IOCTL_SZ_TRANSFER*  piit;
  ULONG               ulLength;
  LONG                lRet                  =  REG_ERROR;  // ERROR
  ULONG               ulReturnedDataLength  =  0;
  //  <0 Error  
  // ==0 OK
  //  >0 Default
  DBG_TRACE_IN(DBG_LVL_FLOW0,lRegGetValueBinary);

  ASSERTDD(hDriver != NULL, "lRegGetValueBinary");
  ASSERTDD(pjData  != NULL, "lRegGetValueBinary");
  ASSERTDD(nByte   != 0,    "lRegGetValueBinary");

  ulLength  =  sizeof(IOCTL_SZ_TRANSFER) + nByte;

  piit = EngAllocMem( FL_ZERO_MEMORY, ulLength, ALLOC_TAG);
  //  call IOCLTRL vstatIoctlRegGetValueBinary
  //
  if( piit )
  {
    strcpy( piit->szId, pstrValue );
    piit->dwDataSize  = nByte;
    piit->dwDataType  = REG_BINARY;

    lRet = (LONG)EngDeviceIoControl( hDriver,
                                     IOCTL_VIDEO_REG_SZ_TRANSFER_GET,
                                     (VOID*)(piit),sizeof(IOCTL_SZ_TRANSFER),
                                     (VOID*)(piit),ulLength,
                                     &ulReturnedDataLength);

    RtlCopyMemory( pjData, piit->ajData, nByte );

    EngFreeMem( piit );
  }

  DBG_TRACE_OUT(DBG_LVL_FLOW0,lRet);

  return lRet;
}

//*******************************************************************************************
// 091098 MSchwarz 
LONG lRegSetValueBinary( HANDLE hDriver, PSTR pstrValue, BYTE* pjData, DWORD nByte )
{

  IOCTL_SZ_TRANSFER*  piit;
  ULONG               ulLength;
  LONG                lRet                =  REG_ERROR;
  ULONG               ReturnedDataLength  =  0;
  //  <0 Error  
  // ==0 OK
  //  >0 Default
  DBG_TRACE_IN(DBG_LVL_FLOW0,lRegSetValueBinary);

  //  call IOCTL 
  ASSERTDD(hDriver != NULL, "lRegSetValueBinary");
  ASSERTDD(pjData  != NULL, "lRegSetValueBinary");
  ASSERTDD(nByte   != 0,    "lRegSetValueBinary");

  ulLength = sizeof(IOCTL_SZ_TRANSFER) + nByte;

  piit = EngAllocMem( FL_ZERO_MEMORY, ulLength, ALLOC_TAG);
  //  call IOCLTRL vstatIoctlRegSetValueBinary
  //
  if( piit )
  {
    strcpy( piit->szId, pstrValue );
    piit->dwDataSize  = nByte;
    piit->dwDataType  = REG_BINARY;
    RtlCopyMemory( piit->ajData, pjData, nByte );

    lRet = (LONG)EngDeviceIoControl( hDriver,
                                     IOCTL_VIDEO_REG_SZ_TRANSFER_SET,
                                     (VOID*)(piit),ulLength,
                                     (VOID*)(piit),0,
                                     &ReturnedDataLength);
    EngFreeMem( piit );
  }

  DBG_TRACE_OUT(DBG_LVL_FLOW0,lRet);

  return lRet;
}

//*******************************************************************************************
// 091098 MSchwarz 
LONG lRegGetValueDWord( HANDLE hDriver, PSTR pstrValue, DWORD* pulData )
{
  IOCTL_SZ_TRANSFER*  piit;
  ULONG               ulLength;
  LONG                lRet                  =  REG_ERROR;  // ERROR
  ULONG               ulReturnedDataLength  =  0;
  //  <0 Error  
  // ==0 OK
  //  >0 Default
  DBG_TRACE_IN(DBG_LVL_FLOW0|DBG_LVL_NOLINEFEED,lRegGetValueDWord);
  DISPDBG((DBG_LVL_FLOW0|DBG_LVL_NOPREFIX," pstrValue:%s",pstrValue));

  ASSERTDD(hDriver != NULL, "lRegGetValueDWord");
  ASSERTDD(pulData != NULL, "lRegGetValueDWord");

  ulLength  =  sizeof(IOCTL_SZ_TRANSFER) + sizeof(DWORD);

  piit = EngAllocMem( FL_ZERO_MEMORY, ulLength, ALLOC_TAG);
  //  call IOCLTRL vstatIoctlRegGetValueDWord
  //
  if( piit )
  {
    strcpy( piit->szId, pstrValue );
    piit->dwDataSize  = sizeof(DWORD);
    piit->dwDataType  = REG_DWORD;

    lRet = (LONG)EngDeviceIoControl( hDriver,
                                     IOCTL_VIDEO_REG_SZ_TRANSFER_GET,
                                     (VOID*)(piit),sizeof(IOCTL_SZ_TRANSFER),
                                     (VOID*)(piit),ulLength,
                                     &ulReturnedDataLength);

    RtlCopyMemory( pulData, piit->ajData, sizeof(DWORD) );

    EngFreeMem( piit );
  }

  DBG_TRACE_OUT(DBG_LVL_FLOW0,lRet);

  return lRet;
}

//*******************************************************************************************
// 121098 MSchwarz 
LONG lRegSetValueDWord( HANDLE hDriver, PSTR pstrValue, DWORD* pulData )
{
  IOCTL_SZ_TRANSFER*  piit;
  ULONG               ulLength;
  LONG                lRet                =  REG_ERROR;
  ULONG               ReturnedDataLength  =   0;
  //  <0 Error  
  // ==0 OK
  //  >0 Default
  DBG_TRACE_IN(DBG_LVL_FLOW0,lRegSetValueDWord);

  ASSERTDD(hDriver != NULL, "lRegSetValueDWord");
  ASSERTDD(pulData != NULL, "lRegSetValueDWord");

  ulLength = sizeof(IOCTL_SZ_TRANSFER) + sizeof(DWORD);

  piit = EngAllocMem( FL_ZERO_MEMORY, ulLength, ALLOC_TAG);
  //  call IOCLTRL vstatIoctlRegSetValueDWord
  //
  if( piit )
  {
    strcpy( piit->szId, pstrValue );
    piit->dwDataSize  = sizeof(DWORD);
    piit->dwDataType  = REG_DWORD;
    RtlCopyMemory( piit->ajData, pulData, sizeof(DWORD) );

    lRet = (LONG)EngDeviceIoControl( hDriver,
                                     IOCTL_VIDEO_REG_SZ_TRANSFER_SET,
                                     (VOID*)(piit),ulLength,
                                     (VOID*)(piit),0,
                                     &ReturnedDataLength);
    EngFreeMem( piit );
  }

  DBG_TRACE_OUT(DBG_LVL_FLOW0,lRet);

  return lRet;
}

//*******************************************************************************************
// 141098 MSchwarz 
LONG lRegGetValueSz( HANDLE hDriver, PSTR pstrValue, PSTR pstrData, DWORD nByte )
{
  IOCTL_SZ_TRANSFER*  piit;
  ULONG               ulLength;
  LONG                lRet                  =  REG_ERROR;  // ERROR
  ULONG               ulReturnedDataLength  =   0;
  //  <0 Error  
  // ==0 OK
  //  >0 Default
  DBG_TRACE_IN(DBG_LVL_FLOW0,lRegGetValueSz);

  ASSERTDD(hDriver  != NULL, "lRegGetValueSz");
  ASSERTDD(pstrData != NULL, "lRegGetValueSz");

  ulLength  =  sizeof(IOCTL_SZ_TRANSFER) + nByte;

  piit = EngAllocMem( FL_ZERO_MEMORY, ulLength, ALLOC_TAG);
  //  call IOCLTRL vstatIoctlRegGetValueSz
  if( piit )
  {
    strcpy( piit->szId, pstrValue );
    piit->dwDataSize  = nByte;
    piit->dwDataType  = REG_SZ;

    lRet = (LONG)EngDeviceIoControl( hDriver,
                                     IOCTL_VIDEO_REG_SZ_TRANSFER_GET,
                                     (VOID*)(piit),sizeof(IOCTL_SZ_TRANSFER),
                                     (VOID*)(piit),ulLength,
                                     &ulReturnedDataLength);

    RtlCopyMemory( pstrData, piit->ajData, nByte );

    EngFreeMem( piit );
  }

  DBG_TRACE_OUT(DBG_LVL_FLOW0,lRet);

  return lRet;
}

//*******************************************************************************************
// 141098 MSchwarz 
LONG lRegSetValueSz( HANDLE hDriver, PSTR pstrValue, PSTR pstrData, DWORD nByte )
{
  IOCTL_SZ_TRANSFER*  piit;
  ULONG               ulLength;
  LONG                lRet                =  REG_ERROR;
  ULONG               ReturnedDataLength  =   0;
  //  <0 Error  
  // ==0 OK
  //  >0 Default
  DBG_TRACE_IN(DBG_LVL_FLOW0,lRegSetValueSz);

  ASSERTDD(hDriver  != NULL, "lRegSetValueSz");
  ASSERTDD(pstrData != NULL, "lRegSetValueSz");

  ulLength = sizeof(IOCTL_SZ_TRANSFER) + nByte;

  piit = EngAllocMem( FL_ZERO_MEMORY, ulLength, ALLOC_TAG);
  //  call IOCLTRL vstatIoctlRegSetValueSz
  //
  if( piit )
  {
    strcpy( piit->szId, pstrValue );
    piit->dwDataSize  = nByte;
    piit->dwDataType  = REG_SZ;
    RtlCopyMemory( piit->ajData, pstrData, nByte );

    lRet = (LONG)EngDeviceIoControl( hDriver,
                                     IOCTL_VIDEO_REG_SZ_TRANSFER_SET,
                                     (VOID*)(piit),ulLength,
                                     (VOID*)(piit),0,
                                     &ReturnedDataLength);
    EngFreeMem( piit );
  }

  DBG_TRACE_OUT(DBG_LVL_FLOW0,lRet);

  return lRet;
}

//*******************************************************************************************
// 141098 MSchwarz 
LONG lRegGetValueMultiSz( HANDLE hDriver, PSTR pstrValue, PSTR pstrData, DWORD nByte )
{
  IOCTL_SZ_TRANSFER*  piit;
  ULONG               ulLength;
  LONG                lRet                  =  REG_ERROR;  // ERROR
  ULONG               ulReturnedDataLength  =   0;
  //  <0 Error  
  // ==0 OK
  //  >0 Default
  DBG_TRACE_IN(DBG_LVL_FLOW0,lRegGetValueMultiSz);

  ASSERTDD(hDriver  != NULL, "lRegGetValueMultiSz");
  ASSERTDD(pstrData != NULL, "lRegGetValueMultiSz");

  ulLength  =  sizeof(IOCTL_SZ_TRANSFER) + nByte;

  piit = EngAllocMem( FL_ZERO_MEMORY, ulLength, ALLOC_TAG);
  //  call IOCLTRL vstatIoctRegGetValueMultiSz
  //
  if( piit )
  {
    strcpy( piit->szId, pstrValue );
    piit->dwDataSize  = nByte;
    piit->dwDataType  = REG_MULTI_SZ;

    lRet = (LONG)EngDeviceIoControl( hDriver,
                                     IOCTL_VIDEO_REG_SZ_TRANSFER_GET,
                                     (VOID*)(piit),sizeof(IOCTL_SZ_TRANSFER),
                                     (VOID*)(piit),ulLength,
                                     &ulReturnedDataLength);

    RtlCopyMemory( pstrData, piit->ajData, nByte );

    EngFreeMem( piit );
  }

  DBG_TRACE_OUT(DBG_LVL_FLOW0,lRet);

  return lRet;
}

//*******************************************************************************************
// 141098 MSchwarz 
LONG lRegSetValueMultiSz( HANDLE hDriver, PSTR pstrValue, PSTR pstrData, DWORD nByte )
{
  IOCTL_SZ_TRANSFER*  piit;
  ULONG               ulLength;
  LONG                lRet                =  REG_ERROR;
  ULONG               ReturnedDataLength  =   0;
  //  <0 Error  
  // ==0 OK
  //  >0 Default
  DBG_TRACE_IN(DBG_LVL_FLOW0,lRegSetValueMultiSz);

  ASSERTDD(hDriver  != NULL, "lRegSetValueMultiSz");
  ASSERTDD(pstrData != NULL, "lRegSetValueMultiSz");

  ulLength = sizeof(IOCTL_SZ_TRANSFER) + nByte;

  piit = EngAllocMem( FL_ZERO_MEMORY, ulLength, ALLOC_TAG);
  //  call IOCLTRL vstatIoctRegSetValueMultiSz
  if( piit )
  {
    strcpy( piit->szId, pstrValue );
    piit->dwDataSize  = nByte;
    piit->dwDataType  = REG_MULTI_SZ;
    RtlCopyMemory( piit->ajData, pstrData, nByte );

    lRet = (LONG)EngDeviceIoControl( hDriver,
                                     IOCTL_VIDEO_REG_SZ_TRANSFER_SET,
                                     (VOID*)(piit),ulLength,
                                     (VOID*)(piit),0,
                                     &ReturnedDataLength);
    EngFreeMem( piit );
  }

  DBG_TRACE_OUT(DBG_LVL_FLOW0,lRet);

  return lRet;
}
#endif
//*******************************************************************************************

/*
** bRegRead (single board)
**
** Initialize driver with information taken out of registry...
**
** returns TRUE if successful
**
** FNicklis 22.10.98: derived from bPageflipModeUpdate
*/
BOOL bRegRead(PPDEV ppdev)
{
  BOOL  bRet     = FALSE;
  ULONG ulSwitch = FALSE;// used as boolean to read registry

  DBG_TRACE_IN(DBG_LVL_FLOW0, bRegRead);

#if 0 // ELSA-FNicklis 04.07.00: unused
    if ( (1==ppdev->cjPelSize) ||                  // 8 bpp
       (ppdev->cyMemory < (ppdev->cyScreen*2)) ) // front, Z
  {
    // 3D not possible

    DBG_ERROR("No room for SB-OpenGL, disable OpenGL");    
    v3DCapsClear((ULONG)0xFFFFFFFF); // clear all bits!

    v3DCapsClear(FL_3DCAPS_3D_POSSIBLE) ;
    v3DCapsClear(FL_3DCAPS_HARDWARE_ACCELERATION);
  }
  else
  {
    // 3D possible!
    ULONG ulBuffers = 0;     // number of decided buffers
    BOOL  bDouble   = TRUE;  // try double buffering
    BOOL  bPageflip = FALSE; // allow pageflipping
    BOOL  bOverlay  = FALSE; // overlay not default
    BOOL  bStereo   = FALSE; // overlay not default

    // We have an accelerated board but...
    v3DCapsSet(FL_3DCAPS_3D_POSSIBLE);

    // ...perhaps it is disabled by the user 
    if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_ICDINTERFACE_DISABLE, &ulSwitch ) )
    {
      ulSwitch = TRUE; // default is 3D on
    }

    // disable != 0 (TRUE) -> not enabled
    if (0 != ulSwitch)
    {
      // 3D acceleration disabled
      v3DCapsClear(FL_3DCAPS_HARDWARE_ACCELERATION);
    }
    else
    {
      // 3D acceleration enabled
      v3DCapsSet(FL_3DCAPS_HARDWARE_ACCELERATION);
    }

    // Check memory capabilities
    bDouble  = bOpenGLDoubleBufferPossible(ppdev);
    bStereo  = bOpenGLStereoBufferPossible(ppdev);
    bOverlay = bOpenGLOverlayBufferPossible(ppdev);


    // write double buffer state
    if (REG_ERROR == lRegSetIdDWord( ppdev->hDriver, ET_VALUE_DW_ICDINTERFACE_MAXNUMBEROFDISPLAYBUFFERS, (bDouble ? 2 : 1)) )
    {
      DBG_ERROR("lRegSetIdDWord failed: ET_VALUE_DW_ICDINTERFACE_MAXNUMBEROFDISPLAYBUFFERS");
    }

    // Set Pixelformats PFD_SUPPORT_GDI flag?
    if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_ICDINTERFACE_PFD_SUPPORT_GDI, &ulSwitch) )
      ulSwitch = TRUE;

    if (ulSwitch)
      v3DCapsSet(FL_3DCAPS_PFD_SUPPORT_GDI);
    else
      v3DCapsClear(FL_3DCAPS_PFD_SUPPORT_GDI);

    // configure SWBC layer
    if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_ICDINTERFACE_FORCEGDIBROADCASTINGINOGLVIEWS, &ulSwitch) )
      ulSwitch = FALSE;

    if (ulSwitch)
      v3DCapsSet(FL_3DCAPS_FORCEGDIBROADCAST);
    else
      v3DCapsClear(FL_3DCAPS_FORCEGDIBROADCAST);

    // check double buffer user settings
    if (bDouble)
    {
      if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_ICDINTERFACE_NUMBEROFDISPLAYBUFFERS, &ulBuffers ) )
        ulBuffers = 2;
    }
    else
      ulBuffers = 1;

    // check double buffer capabilities
    if (ulBuffers > 1)
    {
      v3DCapsSet(FL_3DCAPS_PFD_DOUBLEBUFFER);

      if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_OPENGL_ALIGNED_SURFACES, &ulSwitch ) )
        ulSwitch = 0;
      if (0 != ulSwitch)
        v3DCapsSet(FL_3DCAPS_ALIGNED_SURFACES);
      else
        v3DCapsClear(FL_3DCAPS_ALIGNED_SURFACES);
        
      // check ForceSingleBuffered
      if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_OPENGL_FORCE_SINGLEBUFFERED, &ulSwitch ) )
        ulSwitch = 0;
      if (0 != ulSwitch)
        v3DCapsSet(FL_3DCAPS_FORCE_SINGLEBUFFERD);
      else
        v3DCapsClear(FL_3DCAPS_FORCE_SINGLEBUFFERD);

      //
      // We always support blitted swap buffers, but do we need PFDs that
      // only can blit?
      //
      // Check if we export PFD_SWAP_COPY DrvDescribePixelformat
      //
      if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_ICDINTERFACE_PFD_SWAP_COPY, &ulSwitch) )
        ulSwitch=1;
      if (ulSwitch)
        v3DCapsSet(FL_3DCAPS_PFD_SWAP_COPY);
      else
        v3DCapsClear(FL_3DCAPS_PFD_SWAP_COPY);

#ifdef RUN_WITH_NVIDIA_MINIPORT 
      bPageflip = FALSE;
#else //RUN_WITH_NVIDIA_MINIPORT 
      //
      // Check if we allow pageflipping, else we would force buffer blits
      //
      // ulSwitch = 0 on Riva TNT! // cannot do HBlank-flip on NV4, so disable pageflipping
      if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_ICDINTERFACE_PAGEFLIPPING, &ulSwitch ) )
        ulSwitch = 1; // allow pageflip

      if (0!=ulSwitch)
        bPageflip = TRUE;
#endif//RUN_WITH_NVIDIA_MINIPORT 
    }
    else
    {
      // double buffering disabled
      bDouble  = FALSE; // no double
      v3DCapsClear( FL_3DCAPS_PFD_DOUBLEBUFFER
                   |FL_3DCAPS_PFD_SWAP_COPY);
    }

    //
    // Check vblank user settings
    //
    if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_ICDINTERFACE_VBLANKWAIT, &ulSwitch ) )
      ulSwitch = FALSE;

    if (ulSwitch)
      v3DCapsSet(FL_3DCAPS_VBLANKWAIT);           // wait for vblank
    else
      v3DCapsClear(FL_3DCAPS_VBLANKWAIT);

    //
    // Set page flip capabilities
    //
    if (bPageflip)
    {
      // Pageflipping allowed and possible - clear the force buffer blit bit
      v3DCapsSet(FL_3DCAPS_PAGEFLIP);          // try a pageflip in swap buffers

      // export PFD_SWAP_EXCHANGE in DrvDescribePixelformat?
      if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_ICDINTERFACE_PFD_SWAP_EXCHANGE, &ulSwitch) )
        ulSwitch = 1;
      if (ulSwitch)
      { // export Swap_exchange in pixelformat
        v3DCapsSet(FL_3DCAPS_PFD_SWAP_EXCHANGE);
      }
      else
      {
        v3DCapsClear(FL_3DCAPS_PFD_SWAP_EXCHANGE);
      }
    }
    else
    {
      // not possible to do pagefliping or tripple buffering
      v3DCapsClear( FL_3DCAPS_PAGEFLIP
                   |FL_3DCAPS_PFD_SWAP_EXCHANGE);
    }

    // Check OpenGL DMA Flush -> don't flip or blit
    if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_OPENGL_DISABLEDMAFLUSHES, &ulSwitch ) )
      ulSwitch = 0;

    // Disable ICD rendering: Bits:0x1==ICD only, 2==Display driver
    if (ulSwitch&0x2) 
      v3DCapsSet(FL_3DCAPS_NODMAFLUSH);
    else
      v3DCapsClear(FL_3DCAPS_NODMAFLUSH);

    // Export Buffer Region extension?
    if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_KTX_BUFFERREGIONS, &ulSwitch ) )
      ulSwitch = 1;

    // Disable ICD rendering: Bits:0x1==ICD only, 2==Display driver
    if (ulSwitch) 
      v3DCapsSet(FL_3DCAPS_KTXBUFFERREGIONS);
    else
      v3DCapsClear(FL_3DCAPS_KTXBUFFERREGIONS);


    // Export Buffer Region extension?
    if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_KTX_SWAPHINTS, &ulSwitch ) )
      ulSwitch = 1;
    // Disable ICD rendering: Bits:0x1==ICD only, 2==Display driver
    if (ulSwitch) 
      v3DCapsSet(FL_3DCAPS_KTXSWAPHINTS);
    else
      v3DCapsClear(FL_3DCAPS_KTXSWAPHINTS);


    if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_OPENGL_DISABLE_STENCIL, &ulSwitch ) )
      ulSwitch = 0; // don't disable stencil by default
    if (ulSwitch)
      v3DCapsSet(FL_3DCAPS_PFD_DISABLE_STENCIL);
    else
      v3DCapsClear(FL_3DCAPS_PFD_DISABLE_STENCIL);


    if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_OPENGL_DISABLE_ALPHA, &ulSwitch ) )
      ulSwitch = 0; // don't disable stencil by default
    if (ulSwitch)
      v3DCapsSet(FL_3DCAPS_PFD_DISABLE_ALPHA);
    else
      v3DCapsClear(FL_3DCAPS_PFD_DISABLE_ALPHA);

    //
    // Set stereo capabilities
    //
    if (REG_ERROR == lRegGetIdDWord(ppdev->hDriver, ET_VALUE_DW_ICDINTERFACE_STEREOSUPPORT, &ulSwitch) )
      ulSwitch = 0; // don't use stereo by default

    if(    (bStereo)
        && (0!=ulSwitch) 
      )
    {
      v3DCapsSet(FL_3DCAPS_PFD_STEREO);

      if (REG_ERROR == lRegGetIdDWord(ppdev->hDriver, ET_VALUE_DW_ICDINTERFACE_STEREOFORCEBUFFERS, &ulSwitch) )
        ulSwitch = 0; // don't force stereo buffers by default
      if( 0!=ulSwitch )
        v3DCapsSet(FL_3DCAPS_FORCE_STEREO);
      else
        v3DCapsClear(FL_3DCAPS_FORCE_STEREO);
    }
    else
    {
      v3DCapsClear(FL_3DCAPS_PFD_STEREO);
      v3DCapsClear(FL_3DCAPS_FORCE_STEREO);
    }

    //
    // Set overlay capabilities
    //
    if (REG_ERROR == lRegGetIdDWord(ppdev->hDriver, ET_VALUE_DW_ICDINTERFACE_SUPPORTOVERLAYPLANES, &ulSwitch) )
      ulSwitch = 0; // don't use overlay by default

    if (   bOverlay
        && (0!=ulSwitch) )
    {
      v3DCapsSet(FL_3DCAPS_PFD_OVERLAYS);
      v3DCapsSet(FL_3DCAPS_OVERLAYS_VIA_FRONT); // MSchwarz 2.11.99:  refer to o_overlay.c: 4. 1st comment
      v3DCapsSet(FL_3DCAPS_OVERLAY_SWBC);
      // Set swap layer caps
      if (REG_ERROR == lRegGetIdDWord(ppdev->hDriver, ET_VALUE_DW_ICDINTERFACE_PFD_SWAP_LAYER_BUFFERS, &ulSwitch) )
        ulSwitch = 1; // allow swap layerbuffers by default
      if (ulSwitch)
        v3DCapsSet(FL_3DCAPS_PFD_SWAP_LAYER_BUFFERS);
      else
        v3DCapsClear(FL_3DCAPS_PFD_SWAP_LAYER_BUFFERS);
    }
    else
    {
      if (0!=ulSwitch)
      {
        DBG_ERROR("Want overlay but not enough offscreen");
      }
      v3DCapsClear(FL_3DCAPS_PFD_OVERLAYS);
      v3DCapsClear(FL_3DCAPS_PFD_SWAP_LAYER_BUFFERS);
    }

    if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_ICDINTERFACE_FORCE_STENCIL, &ulSwitch ) )
      ulSwitch = 0; // don't disable stencil by default
    if (0!=ulSwitch)
      v3DCapsSet(FL_3DCAPS_PFD_FORCE_STENCIL);
    else
      v3DCapsClear(FL_3DCAPS_PFD_FORCE_STENCIL);

    if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_ICDINTERFACE_FORCE_MORETHAN16BITZ, &ulSwitch ) )
      ulSwitch = 0; // don't disable stencil by default
    if (0!=ulSwitch)
      v3DCapsSet(FL_3DCAPS_PFD_FORCE_MORETHAN16BITZ);
    else
      v3DCapsClear(FL_3DCAPS_PFD_FORCE_MORETHAN16BITZ);

    //
    // Determine if ICD awaits client or board relative clip rects within shared info
    // Documentaion in o_Clipping.c
    //
    // NOTE: This key has no default, so set it here in all cases!
    v3DCapsClear(FL_3DCAPS_CLIENTRELATIVECLIPRECTS);
    v3DCapsClear(FL_3DCAPS_BOARDRELATIVECLIPRECTS);
    if ( REG_ERROR == lRegGetIdDWord( ppdev->hDriver, INTERNAL_ET_VALUE_DW_OPENGL_CLIENTCLIPRECTS, &ulSwitch) )
    {
      if (bIsSingleScreen(ppdev))
        ulSwitch = DEFAULT_OPENGL_CLIENTCLIPRECTS_1BOARD; // use single board default
      else
        ulSwitch = DEFAULT_OPENGL_CLIENTCLIPRECTS_NBOARD; // use multi boarddefault
    }
    else
    {
      DBG_WARNING("INTERNAL_ET_VALUE_DW_OPENGL_CLIENTCLIPRECTS overwritten by registry!");
    }
    
    switch (ulSwitch)
    {
      case CCT_DESKTOPRELATIVE: 
        // desktop relative clip rects
        break;
      case CCT_BOARDRELATIVE:   
        // board relative
        v3DCapsSet(FL_3DCAPS_BOARDRELATIVECLIPRECTS);
        break;
      case CCT_CLIENTRELATIVE: 
        // client relative
        v3DCapsSet(FL_3DCAPS_CLIENTRELATIVECLIPRECTS);
        break;
      case CCT_CLIENTANDBOARDRELATIVE: 
        // board and client relative
        v3DCapsSet(FL_3DCAPS_BOARDRELATIVECLIPRECTS);
        v3DCapsSet(FL_3DCAPS_CLIENTRELATIVECLIPRECTS);
        break;
      default:
        v3DCapsSet(FL_3DCAPS_BOARDRELATIVECLIPRECTS);
        v3DCapsSet(FL_3DCAPS_CLIENTRELATIVECLIPRECTS);
        ASSERTDD(DEFAULT_OPENGL_CLIENTCLIPRECTS_1BOARD==ulSwitch, "set to client and board relative");
        break;
    }

    #if BETA_VERSION
    if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, INTERNAL_ET_VALUE_DW_OPENGL_NO_ICD_CALLBACK, &ulSwitch ) )
      ulSwitch = 0;
    if (0!=ulSwitch)
      v3DCapsSet(FL_3DCAPS_NO_ICD_CALLBACK);  
    else
      v3DCapsClear(FL_3DCAPS_NO_ICD_CALLBACK);
    #endif    

    #ifdef DBG
      if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, INTERNAL_ET_VALUE_DW_DBG_OPENGLBUFFERNO, &ulSwitch ) )
        ulSwitch = FALSE;

      if (ulSwitch)
        v3DCapsSet(FL_3DCAPS_DBG_PRINTBUFFERNUMBER);  
      else
        v3DCapsClear(FL_3DCAPS_DBG_PRINTBUFFERNUMBER);
    #endif
    vDbg_Printfl3DCapabilities(DBG_LVL_FLOW0, "ppdev->fl3DCapabilities", ppdev);
  }

#ifdef DBG
#ifdef _WIN64
  v3DCapsSet(FL_3DCAPS_DBG_PRINTBUFFERNUMBER);
#endif

  if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, INTERNAL_ET_VALUE_DW_DBG_CHECKHEAPINTEGRITY, (DWORD *)&bDbg_CheckHeap) )
    bDbg_CheckHeap = TRUE;

  DBG_PRINTB(DBG_LVL_FLOW0, bDbg_CheckHeap);
#endif

#if BETA_VERSION
  if ( REG_ERROR == lRegGetIdDWord( ppdev->hDriver, INTERNAL_ET_VALUE_DW_DBG_SURFACENEVERATBOTTOM, &ulSwitch) )
    ulSwitch = 0;

  if (0!=ulSwitch)
  {
    DBG_WARNING("We override the Heaps HEAP_ALLOC_TYPE_POS_BOTTOM flag now!");
    v2DCapsSet(FL_2DCAPS_SURFACENEVERATBOTTOM);
  }
  else
    v2DCapsClear(FL_2DCAPS_SURFACENEVERATBOTTOM);
  
  if ( REG_ERROR == lRegGetIdDWord( ppdev->hDriver, INTERNAL_ET_VALUE_DW_HELPERFLAGS, &ppdev->flRegHelperFlags) )
    ppdev->flRegHelperFlags = 0;

  // Read reserved keys for internal and temporary usage.
  // Never check in code that uses ulRegReserved0-9!
  if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, INTERNAL_ET_VALUE_DW_RESERVED0, &ppdev->ulRegReserved0) )
    ppdev->ulRegReserved0 = 0;
  if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, INTERNAL_ET_VALUE_DW_RESERVED1, &ppdev->ulRegReserved1) )
    ppdev->ulRegReserved1 = 0;
  if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, INTERNAL_ET_VALUE_DW_RESERVED2, &ppdev->ulRegReserved2) )
    ppdev->ulRegReserved2 = 0;
  if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, INTERNAL_ET_VALUE_DW_RESERVED3, &ppdev->ulRegReserved3) )
    ppdev->ulRegReserved3 = 0;
  if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, INTERNAL_ET_VALUE_DW_RESERVED4, &ppdev->ulRegReserved4) )
    ppdev->ulRegReserved4 = 0;
  if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, INTERNAL_ET_VALUE_DW_RESERVED5, &ppdev->ulRegReserved5) )
    ppdev->ulRegReserved5 = 0;
  if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, INTERNAL_ET_VALUE_DW_RESERVED6, &ppdev->ulRegReserved6) )
    ppdev->ulRegReserved6 = 0;
  if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, INTERNAL_ET_VALUE_DW_RESERVED7, &ppdev->ulRegReserved7) )
    ppdev->ulRegReserved7 = 0;
  if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, INTERNAL_ET_VALUE_DW_RESERVED8, &ppdev->ulRegReserved8) )
    ppdev->ulRegReserved8 = 0;
  if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, INTERNAL_ET_VALUE_DW_RESERVED9, &ppdev->ulRegReserved9) )
    ppdev->ulRegReserved9 = 0;
#endif

  if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_DDRAWSUPPORT_OVERLAYSHRINK, &ulSwitch ) )
    ulSwitch = 0;
  
  //  Skuklik: 09.02.2000
  //  0002.0115.5432.CBUE
  //  to fix misbehavior with mediamatics DVD Express
  //  Enabling support for OverlayShrink in DDraw causes fails in HCT (TDDraw) 
  //  CASE 12: Consume all Vid Mem Test (TC 6.1.1) fails
  //  So just turn on this feature if needed by an application, and disable it by default

  if (0!=ulSwitch)
    v2DCapsSet(FL_2DCAPS_SUPPORT_OVERLAYSHRINK);  
  else
    v2DCapsClear(FL_2DCAPS_SUPPORT_OVERLAYSHRINK);

  vDbg_Printfl2DCapabilities(DBG_LVL_FLOW0, "ppdev->fl2DCapabilities", ppdev);

  bRet = TRUE;
#endif // ELSA-FNicklis 04.07.00: unused

  DBG_TRACE_OUT(DBG_LVL_FLOW0, bRet);
  return (bRet);
}

#if DBG
// ELSA-FNicklis 14.04.00: global to help masking of punt flags below.
FLONG flDbgPunt=0xFFFFFFFF;
#endif

/*
** bRegInit
**
** Initialize registry stuff and read driver init values.
**
** FNicklis 27.05.1999: Moved DrvEnableSurface stuff to this function.
*/
BOOL bRegInit(PPDEV ppdev)
{
  ULONG ulSwitch;
  BOOL  bRet = FALSE;

#if 0 // ELSA-FNicklis 04.07.00: unused
    ASSERT(NULL!=ppdev);
  ASSERT(NULL!=ppdev->hDriver);

  // MSchwarz 161098 moved it from RegRead because EnableHardware needs the value
  if(REG_ERROR == lRegGetIdDWord(ppdev->hDriver, ET_VALUE_DW_GDI_DMABUFFERSIZE, &ppdev->DmaPushBufTotalSize))
  {
    ppdev->DmaPushBufTotalSize = DMAPUSH_BUFFER_SIZE;
    DISPDBG((DBG_LVL_ERROR, "ERROR in %s failed to read %s: ppdev->DmaPushBufTotalSize: 0x%x",
      szDbg_TRACE_GetCurrentFn(),
      szDbg_GetET_VALUE_IDName(ET_VALUE_DW_GDI_DMABUFFERSIZE), 
      ppdev->DmaPushBufTotalSize));
  }

  // Need at least 8kb buffer to run properly
  if (ppdev->DmaPushBufTotalSize < ((8 KB) + sizeof(DWORD)))
    ppdev->DmaPushBufTotalSize = DMAPUSH_BUFFER_SIZE;

  if(REG_ERROR == lRegGetIdDWord(ppdev->hDriver, ET_VALUE_DW_GDI_DMAPADDING, &ppdev->ulDmaPadding))
  {
    ppdev->ulDmaPadding = DMAPUSH_PADDING;
    DISPDBG((DBG_LVL_ERROR, "ERROR in %s failed to read %s: ppdev->DmaPushBufTotalSize: 0x%x",
      szDbg_TRACE_GetCurrentFn(),
      szDbg_GetET_VALUE_IDName(ET_VALUE_DW_GDI_DMAPADDING), 
      ppdev->ulDmaPadding));
  }

  // at least one DWORD at the end
  if (ppdev->ulDmaPadding < sizeof(DWORD))
    ppdev->ulDmaPadding = sizeof(DWORD);

  // Need at least 8kb buffer to run properly
  if (ppdev->DmaPushBufTotalSize-ppdev->ulDmaPadding < (8 KB))
  {
    DBG_ERROR("Fallback to default DmaPushBufTotalSize and ulDmaPadding");
    ppdev->ulDmaPadding    = DMAPUSH_PADDING;
    ppdev->DmaPushBufTotalSize = DMAPUSH_BUFFER_SIZE;
  }


  //
  // Support HW lines
  //
  if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_GDI_HWLINES, &ulSwitch) )
    ulSwitch = FALSE;
  if (ulSwitch)
    v2DCapsSet(FL_2DCAPS_HWLINES);  
  else
    v2DCapsClear(FL_2DCAPS_HWLINES);


  //
  // Enable device bitmaps
  //
#ifdef _WIN64
  // FNicklis IA-64: ToDo - portation not complete!
  #pragma message ("  FNicklis IA-64: ToDo: disabled FL_2DCAPS_ENABLEDEVICEBITMAPS "__FILE__)
  DISPDBG((DBG_LVL_IA64, "FNicklis IA-64: ToDo: disabled FL_2DCAPS_ENABLEDEVICEBITMAPS %s %d",__FILE__,__LINE__));
  v2DCapsClear(FL_2DCAPS_ENABLEDEVICEBITMAPS);
#else
#ifndef RUN_WITH_NVIDIA_MINIPORT 
  if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_GDI_ENABLEDEVICEBITMAPS, &ulSwitch) )
    ulSwitch = TRUE;
  if (ulSwitch)
    v2DCapsSet(FL_2DCAPS_ENABLEDEVICEBITMAPS);
  else
#endif //RUN_WITH_NVIDIA_MINIPORT 
  {
    DBG_WARNING("FNicklis IA-64: ToDo: disabled FL_2DCAPS_ENABLEDEVICEBITMAPS");
    v2DCapsClear(FL_2DCAPS_ENABLEDEVICEBITMAPS);
  }
#endif


  //
  // Read GDI DMA-buffer allocation type
  //
#if (_WIN32_WINNT >= 0x0500)
  DISPDBG((DBG_LVL_FLOW0, "AGP needed for W2K initialization of miniport"));
  v2DCapsSet(FL_2DCAPS_AGP_DMA);

#else
  if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_GDI_AGP_DMA, &ulSwitch) )
  {
    ulSwitch = FALSE;
    DISPDBG((DBG_LVL_ERROR, "ERROR in %s failed to read %s: ulSwitch: %s",
      szDbg_TRACE_GetCurrentFn(),
      szDbg_GetET_VALUE_IDName(ET_VALUE_DW_GDI_AGP_DMA), 
      szBOOL(ulSwitch)));
  }
  if (ulSwitch)
    v2DCapsSet(FL_2DCAPS_AGP_DMA);
  else
    v2DCapsClear(FL_2DCAPS_AGP_DMA);
#endif // ELSA-FNicklis 09.05.00: unused


  if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, ET_VALUE_DW_GDI_FRAMEBUFFER_DMA, &ulSwitch) )
  {
    ulSwitch = FALSE;
    DISPDBG((DBG_LVL_ERROR, "ERROR in %s failed to read %s: ulSwitch: %s",
      szDbg_TRACE_GetCurrentFn(),
      szDbg_GetET_VALUE_IDName(ET_VALUE_DW_GDI_FRAMEBUFFER_DMA), 
      szBOOL(ulSwitch)));
  }
  if (ulSwitch)
    v2DCapsSet(FL_2DCAPS_FRAMEBUFFER_DMA);
  else
    v2DCapsClear(FL_2DCAPS_FRAMEBUFFER_DMA);

  vDbg_Printfl2DCapabilities(DBG_LVL_FLOW0, "ppdev->fl2DCapabilities", ppdev);

#if BETA_VERSION
#ifdef RUN_WITH_NVIDIA_MINIPORT 
      DISPDBG((DBG_LVL_WARNING, "ELSA-FNicklis 14.04.00: ppdev->flPunt %s %d",__FILE__,__LINE__));
      ppdev->flPunt = (
                      FL_PUNT_DRVFILLPATH          
                    | FL_PUNT_DRVCOPYBITS          
                    | FL_PUNT_DRVBITBLT                                 
                    | FL_PUNT_DRVLINETO            
                    | FL_PUNT_DRVPAINT             
                    | FL_PUNT_DRVSTRETCHBLT        
                    | FL_PUNT_DRVSTROKEPATH        
                    | FL_PUNT_DRVTEXTOUT           
                    | FL_PUNT_DRVMOVEPOINTER       
                    | FL_PUNT_DRVSETPOINTERSHAPE   
                    | FL_PUNT_DRVCREATEDEVICEBITMAP
                    | FL_PUNT_DRVSETPALETTE                
                    | FL_PUNT_CHANNELSWITCH
                    | FL_PUNT_SYNCH
                    ) 
#if DBG
                    & flDbgPunt
#endif
                    ;
#else
  // FNicklis IA-64: Added dynamic punt possiblity
  if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, INTERNAL_ET_VALUE_DW_GDI_PUNTFLAGS, &ppdev->flPunt) )
  {
    #ifdef _WIN64
      // FNicklis IA-64: ToDo - portation not complete!
      #pragma message ("  FNicklis IA-64: ToDo: ppdev->flPunt "__FILE__)
      DISPDBG((DBG_LVL_IA64, "FNicklis IA-64: ToDo: ppdev->flPunt %s %d",__FILE__,__LINE__));
      ppdev->flPunt = FL_PUNT_DRVFILLPATH          
                    | FL_PUNT_DRVCOPYBITS          
                    | FL_PUNT_DRVBITBLT                                 
                    | FL_PUNT_DRVLINETO            
                    | FL_PUNT_DRVPAINT             
                    | FL_PUNT_DRVSTRETCHBLT        
                    | FL_PUNT_DRVSTROKEPATH        
                    | FL_PUNT_DRVTEXTOUT           
                    | FL_PUNT_DRVMOVEPOINTER       
                    | FL_PUNT_DRVSETPOINTERSHAPE   
                    | FL_PUNT_DRVCREATEDEVICEBITMAP
                    | FL_PUNT_DRVSETPALETTE                
                    | FL_PUNT_CHANNELSWITCH
                    | FL_PUNT_SYNCH
                    ;
    #else
      ppdev->flPunt = 0; // don't punt anything
    #endif
  }
#endif
  if (0!=ppdev->flPunt)
  {
    vDbg_PrintFL_PUNT_Flags(DBG_LVL_WARNING, "ppdev->flPunt", ppdev->flPunt);
  }
#endif


  if (REG_ERROR == lRegGetIdDWord( ppdev->hDriver, INTERNAL_ET_VALUE_DW_OPENGL_OVERLAYBPP, &ppdev->ulOverlayBPP) )
  {
    ASSERT(0!=ppdev->cjPelSize);
    ppdev->ulOverlayBPP = ppdev->cjPelSize * 8; // bits per pixel => TC overlay in TC, 16bpp overlay in 16bpp
  }
  else
  {
    DISPDBG((DBG_LVL_WARNING, "Overwrite for INTERNAL_ET_VALUE_DW_OPENGL_OVERLAYBPP:%d", ppdev->ulOverlayBPP));
  }

  bRet = TRUE;
#endif // ELSA-FNicklis 04.07.00: unused
  return(bRet);
}


/*
** bRegQueryAgainstInternalStatus (single board)
**
** Esc interface to retrieve any driver handled registry key
**
** NOTE: Don't disable keys using the flags set by the keys! Else 
**       yout wont be able to enable again!
**
** plRet : ESC_SUCCESS      ( 1) : function finished successfully
**         ESC_NOTSUPPORTED ( 0) : ERROR: iEsc or SubEsc not supported
**         ESC_ERROR        (-1) : ERROR: any other error (wrong feeding)
** return: TRUE                  : Escape handled, plRet and pvOut valid
**         FALSE                 : Escape not handled, do it through registry
**
** FNicklis 05.03.1999: New
** FNicklis 25.10.1999: Added first valid return values
*/
static BOOL bRegQueryAgainstInternalStatus(
      PPDEV           ppdev, 
  IN  DWORD           dwSubEsc, 
  IN  ET_QUERY_VALUE *petQueryValue,
  IN  ULONG           cjOut, 
  OUT VOID           *pvOut,
  OUT LONG           *plRet)
{
  ET_VALUE_DWORD *petValueDWord = (ET_VALUE_DWORD*)pvOut;
  LONG            bRet          = FALSE; // not handled (default)
  DWORD           dwType;                // recalulated and modified type identifier
  BOOL            bDisableKey   = FALSE; // Key disabled 
  BOOL            bValueSet     = FALSE; // We have a value to return, stored to petValueDWord->dwValue

  DBG_TRACE_IN(DBG_LVL_FLOW0_NLF,bRegQueryAgainstInternalStatus);
  ASSERT(NULL!=petQueryValue);
  DBG_PRINTX(DBG_LVL_FLOW0_NP, petQueryValue->dwID);

  ASSERT(NULL!=ppdev);

  if (dwSubEsc==SUBESC_ET_QUERY_VALUE_DWORD)
  {
    // Don't touch output before we have checked it!
    if (   ( NULL==pvOut )
        || ( sizeof(ET_VALUE_DWORD)!= cjOut )
        || ( sizeof(ET_VALUE_DWORD)!= petQueryValue->hdr.dwOutSize )  )
    {
      DBG_ERROR("Outbuffer to small or invalid data:");
      DISPDBG((DBG_LVL_ERROR, "sizeof(ET_VALUE_DWORD)=%d, cjOut=%d, petQueryValue->hdr.dwOutSize:%d", sizeof(ET_VALUE_DWORD), cjOut,petQueryValue->hdr.dwOutSize));
      bRet   = TRUE;      // handled, but cannot work correctly
      *plRet = ESC_ERROR; // return error to escape
      goto Exit;
    }
    // Ok
  }
  else
  {
    //
    // Only handle dwords by now!
    //
    goto Exit;
  }

  // check if we get the default only
  dwType = petQueryValue->dwType;

  // first check the Keys we don't want to support!
  switch (petQueryValue->dwID)
  {
    case ET_VALUE_DW_ICDINTERFACE_DISABLE:
    {
#if 0 // ELSA-FNicklis 04.07.00: unused
      if (!b3DCapsTest(FL_3DCAPS_3D_POSSIBLE))
      {
        bDisableKey=TRUE;
      }
#endif // ELSA-FNicklis 04.07.00: unused
      break;
    }

    // Keys that are not available in single buffered resolutions:
    case ET_VALUE_DW_ICDINTERFACE_PAGEFLIPPING:
    case ET_VALUE_DW_ICDINTERFACE_VBLANKWAIT:
    case ET_VALUE_DW_ICDINTERFACE_PFD_SWAP_COPY:
    case ET_VALUE_DW_ICDINTERFACE_PFD_SWAP_EXCHANGE:
    case ET_VALUE_DW_ICDINTERFACE_FORCEGDIBROADCASTINGINOGLVIEWS:
    case ET_VALUE_DW_ICDINTERFACE_NUMBEROFDISPLAYBUFFERS:
    {
      #if 0 // ELSA-FNicklis 04.07.00: unused
      if (!bOpenGLDoubleBufferPossible(ppdev))
      {
        bDisableKey=TRUE;
      }
      #endif // ELSA-FNicklis 04.07.00: unused
      break;
    }

    // Keys that are not available in non overlay capable resolutions:
    case ET_VALUE_DW_ICDINTERFACE_PFD_SWAP_LAYER_BUFFERS:
    case ET_VALUE_DW_ICDINTERFACE_SUPPORTOVERLAYPLANES  :
    {
      #if 0 // ELSA-FNicklis 04.07.00: unused
      if (!bOpenGLOverlayBufferPossible(ppdev))
      {
        bDisableKey=TRUE;
      }
      #endif // ELSA-FNicklis 04.07.00: unused
      break;
    }

    #if 0 // ELSA-FNicklis 04.07.00: unused
    case INTERNAL_ET_VALUE_DW_OPENGL_OVERLAYBPP         :
    {
      if (!bOpenGLOverlayBufferPossible(ppdev))
      {
        bDisableKey=TRUE;
      }
      else
      {
        petValueDWord->dwValue = ppdev->ulOverlayBPP;
        bValueSet = TRUE;
      }
      break;
    }
    #endif // ELSA-FNicklis 04.07.00: unused

#if 0 // ELSA-FNicklis 04.07.00: unused
    // Keys that are not available in non stereo capable resolutions:
    case ET_VALUE_DW_ICDINTERFACE_PFD_STEREO:
    case ET_VALUE_DW_ICDINTERFACE_STEREOFORCEBUFFERS:
    case ET_VALUE_DW_ICDINTERFACE_STEREOSUPPORT:
    {
      if (!bOpenGLStereoBufferPossible(ppdev))
      {
        bDisableKey=TRUE;
      }
      break;
    }
#endif // ELSA-FNicklis 04.07.00: unused
  }

  if (bValueSet)
  {
    //
    // Did find an internal value, no need to go to registry!
    //
    petValueDWord->dwSize   = sizeof(ET_VALUE_DWORD);
    petValueDWord->dwID     = petQueryValue->dwID;
    petValueDWord->dwFlags  = ET_VALUE_OK;

    *plRet = ESC_SUCCESS; // Handle errors in dwFlags of return structure
    bRet = TRUE;
  }
  else if (bDisableKey)
  {
    // did we find a key want to disable

    // NOTE: Don't disable keys using the flags set by the keys! Else 
    //       yout wont be able to enable again!
    petValueDWord->dwSize   = sizeof(ET_VALUE_DWORD);
    petValueDWord->dwID     = petQueryValue->dwID;
    petValueDWord->dwFlags  = ET_VALUE_UNKNOWN;

    vDbg_PrintET_VALUE_RETdwFlags(DBG_LVL_FLOW1, "petValueDWord->dwFlags",petValueDWord->dwFlags);
    *plRet = ESC_SUCCESS; // Handle errors in dwFlags of return structure
    bRet = TRUE;
  }
  else 
  {
    goto Exit;
  }

  // defaults that have to be handled must go through miniport!
  if (bBitsTest(dwType,ET_VALUE_DEFAULT_FLAG))
  {
    vBitsClear(&dwType,ET_VALUE_DEFAULT_FLAG);
    goto Exit;
  }

  if (ET_VALUE_TYPE_DWORD != dwType) // must be DWord now, as Default flag is removed
  {
    DBG_ERROR("internal error: returning wrong type");
    bRet   = TRUE; // handled, but cannot work correctly
    *plRet = ESC_ERROR;
    goto Exit;
  }
  
Exit:
  DBG_TRACE_OUT(DBG_LVL_FLOW0, bRet);

  return bRet;
}

#endif //USE_WKS_ELSA_TOOLS
// End of wkselsatoolsregistry.c
