//*****************************Module*Header******************************
//
// Module Name: wkselsatoolsescape.c
//
// Handles ESC_ELSA_TOOLS calls to driver
//
// FNicklisch 09/14/2000: New, derived from EscapeTools.c
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
#include "nvVer.h"
#include "excpt.h"
#ifdef USE_WKS_ELSA_TOOLS
#include "ELSAESC.h"
#include "wkselsatoolsdebug.h"
#include "wkselsatoolsregistry.h"
#include "wkselsatoolstoolbox.h"

// export header file
#include "wkselsatoolsescape.h"


#define szStrCpy strcpy
#define szStrCat strcat

static LONG lQueryEtVersionInfo(
  IN  PPDEV            ppdev, 
  OUT ET_VERSION_INFO *pVersionInfo);

static LONG lQueryOpenGlIcdRegPath(
      PPDEV                 ppdev,
  OUT ET_OPENGLICD_REGPATH *pIcdRegPath);

static LONG lEscET_EDD_GETGAMMARAMP(IN const  PPDEV                 ppdev, 
                                    IN const  ET_QUERY*             pET_Query, 
                                    OUT       PET_EDD_GETGAMMARAMP  pETGetGammaRamp);

static LONG lEscET_EDD_SETGAMMARAMP(IN        PPDEV ppdev, 
                                    IN const  PET_EDD_SETGAMMARAMP  pETSetGammaRamp, 
                                    OUT       PET_SET_VALUE_RET     pETSetValueRet);

static LONG lEscET_SET_VALUE_BINARY(IN        PPDEV ppdev, 
                                    IN const  PET_SET_VALUE_BINARY  pETSetValueBinary, 
                                    OUT       PET_SET_VALUE_RET     pETSetValueRet);

static LONG lEscET_DPMS(
  IN     PPDEV             ppdev, 
  IN     ET_DPMS_IN  *pIn, 
     OUT ET_DPMS_OUT *pOut);

static LONG lEscET_QUERY_TEMPERATURE(
  IN    PPDEV                 ppdev, 
  IN    ET_QUERY             *petQuery, 
    OUT ET_QUERY_TEMPERATURE *petTemperature);

/*
** lEscapeTools
**
** handles all escape function calls regarding the ELSA escape interface.
**
** return: ESC_SUCCESS:      ok, returned data valid
**         ESC_NOTSUPPORTED: failed, esc not supported, input data ignored, output data invalid
**         ESC_ERROR:        failed, returned data invalid
*/
LONG lEscapeTools( 
  SURFOBJ *pso  ,
  ULONG    cjIn ,
  VOID    *pvIn ,
  ULONG    cjOut,
  VOID    *pvOut)
{
  LONG  lRet = ESC_NOTSUPPORTED; // Default error 

  DBG_TRACE_IN(DBG_LVL_SUBENTRY, lEscapeTools);
  
  ASSERTDD(NULL!=pso, "lEscapeTools:");

#ifndef RUN_WITH_NVIDIA_MINIPORT 
  // We need a minimum of ET_QUERY to understand the subescape!
  if ( (NULL == pvIn) || 
       (cjIn < sizeof(ET_QUERY)) )
  {
    DBG_ERROR("cjIn<sizeof(ET_QUERY)");
    lRet = ESC_ERROR;
    goto Exit;
  } 
  else 
  {
    ET_QUERY *pEtQuery;
    PPDEV     ppdev;
    DWORD     dwSubEsc;

    ppdev = (struct _PDEV *)pso->dhpdev;

    ASSERT(NULL!=ppdev);

    pEtQuery = (ET_QUERY *)pvIn;
    dwSubEsc = pEtQuery->dwSubEsc;

    DISPDBG((DBG_LVL_SUBENTRY, "dwSubEsc:0x%x (%s), ulBoard:%d", 
      dwSubEsc, szDbg_GetETSubEscName(dwSubEsc), pEtQuery->dwBoard));

    if ( (NULL!=pvOut) && (cjOut>0) )
    {
      if (cjOut!=pEtQuery->dwOutSize)
      {
        DISPDBG((DBG_LVL_ERROR, "ERROR in %s: %d==cjOut != pEtQuery->dwOutSize==%d",
          szDbg_TRACE_GetCurrentFn(),cjOut,pEtQuery->dwOutSize));
        DISPDBG((DBG_LVL_ERROR, "ERROR in %s: dwSubEsc:0x%x (%s), ulBoard:%d", 
          szDbg_TRACE_GetCurrentFn(),dwSubEsc, szDbg_GetETSubEscName(dwSubEsc), pEtQuery->dwBoard));
        lRet = ESC_ERROR;
        goto Exit;
      }

      // clear output buffer before anyone touches it!
      RtlZeroMemory(pvOut, cjOut);
    }

    switch (dwSubEsc)
    {  
      // VERSION_INFO
      // return version nummbers, registry path and 
      case SUBESC_ET_QUERY_VERSION_INFO:
      {
        if ( (NULL==pvOut) || 
             (sizeof(ET_VERSION_INFO) != cjOut) )
        {
          DISPDBG((DBG_LVL_ERROR, "ERROR in DrvEscape: SUBESC_ET_QUERY_VERSION_INFO cjOut:%d != pEtQuery->dwOutSize:%d != sizeof(ET_VERSION_INFO):%d",cjOut, pEtQuery->dwOutSize, sizeof(ET_VERSION_INFO)));
          break;
        }

        lRet = lQueryEtVersionInfo(ppdev, (ET_VERSION_INFO *)pvOut);
        break; 
      }

      case SUBESC_ET_WINMAN2_DATA_SET:
      case SUBESC_ET_WINMAN2_DATA_GET:
      case SUBESC_ET_WINMAN2_DDC_EDID_GET:
      {
        //lRet = lEscWINman2(ppdev, dwSubEsc, cjIn, pvIn, cjOut, pvOut);
        break;
      }

      // APPSET
      // queries ICDs registry path and information about ICD name
      case SUBESC_ET_QUERY_OPENGLICD_REGPATH:
      {
        if ( (NULL==pvOut) || 
             (sizeof(ET_OPENGLICD_REGPATH) > cjOut) ) 
        {
          DBG_ERROR("SUBESC_ET_QUERY_OPENGLICD_REGPATH cjOut to small");
          break;
        }

        // FNicklisch 14.09.00: lRet = lQueryOpenGlIcdRegPath(ppdev, (ET_OPENGLICD_REGPATH *)pvOut);
        break;
      }

      case SUBESC_ET_QUERY_VALUE_BINARY:  
      case SUBESC_ET_QUERY_VALUE_STRING:  
      case SUBESC_ET_QUERY_VALUE_DWORD:   
      case SUBESC_ET_SET_VALUE_STRING:  
      case SUBESC_ET_SET_VALUE_DWORD:  
      //case SUBESC_ET_SET_VALUE_DEFAULT:
      {
        // FNicklisch 14.09.00: lRet = lEscRegistryValue(ppdev, dwSubEsc, cjIn, pvIn, cjOut, pvOut);
        break;
      }

#if 0 // FNicklisch 14.09.00: unused
      // POWERLIB
      case SUBESC_ET_POWERLIB_START:  
      {
        if( !ppdev )
          break;
        // increase number of started powerlibs
        ppdev->pElsaPDEV->nPowerLibsStarted++;
        // switch on hardwarelines, witch won't pass the WHQL
        lRet = ESC_SUCCESS;
        break;
      }

      case SUBESC_ET_POWERLIB_EXIT:  
      {
        ppdev = pPDEVGetFromPso( pso );
        if( !ppdev )
          break;
        // are there any powerlibs left ?
        if( ppdev->pElsaPDEV->nPowerLibsStarted )
          ppdev->pElsaPDEV->nPowerLibsStarted--;
        // if tere are no powerlibs left, clear flag
        lRet = ESC_SUCCESS;
        break;
      }
#endif // FNicklisch 14.09.00: unused

      case SUBESC_ET_QUERY_ICDCLIENTINFO:
      {
        lRet = ESC_ERROR;
        break;
      }

      case SUBESC_ET_EDD_SETGAMMARAMP:

          if((cjIn  == sizeof(ET_EDD_SETGAMMARAMP)) && 
             (cjIn  == pEtQuery->dwSize)            &&
             (pvOut != NULL)                        &&
             (cjOut == sizeof(ET_SET_VALUE_RET))    &&
             (cjOut == pEtQuery->dwOutSize))
          {
            // FNicklisch 14.09.00: lRet = lEscET_EDD_SETGAMMARAMP(ppdev, (ET_EDD_SETGAMMARAMP*)pvIn, (ET_SET_VALUE_RET*)pvOut);
          }
          else
          {
            lRet = ESC_ERROR;
          }
          break;
      
      case SUBESC_ET_EDD_GETGAMMARAMP:

          if((cjIn  == sizeof(ET_QUERY))            &&
             (cjIn  == pEtQuery->dwSize)            &&
             (pvOut != NULL)                        &&
             (cjOut == sizeof(ET_EDD_GETGAMMARAMP)) &&
             (cjOut == pEtQuery->dwOutSize) )
          {
            // FNicklisch 14.09.00: lRet = lEscET_EDD_GETGAMMARAMP(ppdev, (ET_QUERY*)pvIn, (ET_EDD_GETGAMMARAMP*)pvOut);
          }
          else
          {
            lRet = ESC_ERROR;
          }
          break;
      
      case SUBESC_ET_SET_VALUE_BINARY:
          if((cjIn  >= sizeof(ET_SET_VALUE_BINARY_HDR_SIZE))  &&
             (cjIn  == pEtQuery->dwSize)                      &&
             (pvOut != NULL)                                  &&
             (cjOut == sizeof(ET_SET_VALUE_RET))              &&
             (cjOut == pEtQuery->dwOutSize) )
          {
            lRet = lEscET_SET_VALUE_BINARY(ppdev, (ET_SET_VALUE_BINARY*)pvIn, (ET_SET_VALUE_RET*)pvOut);
          }
          else
          {
            lRet = ESC_ERROR;
          }
          break;

      case SUBESC_ET_DPMS:
          if((cjIn  == sizeof(ET_DPMS_IN))  &&
             (cjIn  == pEtQuery->dwSize)         &&
             (pvOut != NULL)                     &&
             (cjOut == sizeof(ET_DPMS_OUT)) &&
             (cjOut == pEtQuery->dwOutSize) )
          {
            // FNicklisch 14.09.00: lRet = lEscET_DPMS(ppdev, (ET_DPMS_IN*)pvIn, (ET_DPMS_OUT*)pvOut);
          }
          else
          {
            lRet = ESC_ERROR;
          }
          break;

      case SUBESC_ET_QUERY_TEMPERATURE:
          if((cjIn  == sizeof(ET_QUERY))  &&
             (cjIn  == pEtQuery->dwSize)         &&
             (pvOut != NULL)                     &&
             (cjOut == sizeof(ET_QUERY_TEMPERATURE)) &&
             (cjOut == pEtQuery->dwOutSize) )
          {
            // FNicklisch 14.09.00: lRet = lEscET_QUERY_TEMPERATURE(ppdev, (ET_QUERY*)pvIn, (ET_QUERY_TEMPERATURE*)pvOut);
          }
          else
          {
            lRet = ESC_ERROR;
          }
          break;

      default:
      {
        DISPDBG((DBG_LVL_WARNING, "WARNING in ulEscapeTools: Unsuported sub escape: %s, 0x%x",szDbg_GetETSubEscName(dwSubEsc),dwSubEsc));
        lRet=ESC_NOTSUPPORTED; // Not supported
        break;
      }
    } // .. switch
  } // .. if cjIn else ..
Exit:
#endif
  if (ESC_ERROR==lRet) 
  {
    EngSetLastError(ERROR_INVALID_PARAMETER); 
  }

  DBG_TRACE_OUT(DBG_LVL_SUBENTRY, lRet);

  return lRet;
}

#ifndef RUN_WITH_NVIDIA_MINIPORT 

//*******************************************************************************************
/*
** lQueryEtVersionInfo
**
** is a tools escape function, that returns the drivers
** registry path, version number and capability flags.
**
** return: ESC_ERROR:   failed, returned data invalid
**         ESC_SUCCESS: ok, returned data valid
*/
#if 1
// local information
#pragma message ("  FNicklis: use the other lQueryEtVersionInfo if finally tested!")
static LONG lQueryEtVersionInfo(
  IN  PPDEV            ppdev, 
  OUT ET_VERSION_INFO *pVersionInfo)
{
  LONG              lRet  = ESC_ERROR;

  DBG_TRACE_IN(DBG_LVL_SUBENTRY,lQueryEtVersionInfo);

  ASSERTDD(NULL!=ppdev,        "");
  ASSERTDD(NULL!=pVersionInfo, "");

  pVersionInfo->dwSize = sizeof(ET_VERSION_INFO);

  szStrCpy( pVersionInfo->szRegDriverKey,    "\\Registry\\Machine\\System\\CurrentControlSet\\Services\\nv4" );
  szStrCpy( pVersionInfo->Version.szVersion, NV_VERSION_MS_STRING_NT4);

  pVersionInfo->Version.dwFlags           = 0;
  #if defined (BETA_VERSION)
  pVersionInfo->Version.dwFlags          |= ET_VER_BETA_DRIVER; 
  #endif
  pVersionInfo->Version.dwNTMajor         = 4;
  pVersionInfo->Version.dwNTMinor         = 0;
  pVersionInfo->Version.dwNTBuildMajor    = 1381;
  pVersionInfo->Version.dwNTBuildMinor    = NV_DRIVER_VERSION_NUMBER;

  // validate capabilities
  pVersionInfo->Version.dwNTVersionStamp  = ET_VERSION_STAMP10;
  // FNicklisch 14.09.00: if ( b2DCapsTest(FL_2DCAPS_CV_BOARD) )
  {
    pVersionInfo->Version.dwNTCapabilities |= ET_VERSION_CAPS_CV_PRODUCT;
  }

  // We always have 3D boards in CV!
  // pVersionInfo->Version.dwNTCapabilities |= ET_VERSION_CAPS_2D_BOARD_ONLY;

  // FNicklisch 14.09.00: vDbg_PrintET_VERSION_INFO(DBG_LVL_FLOW1, "pVersionInfo", pVersionInfo);

  lRet = ESC_SUCCESS;

  DBG_TRACE_OUT(DBG_LVL_SUBENTRY,lRet);

  return lRet;
}

#else
// FNicklis informationvia IOCTL
static LONG lQueryEtVersionInfo(
  IN  PPDEV            ppdev, 
  OUT ET_VERSION_INFO *pVersionInfo)
{
  LONG  lRet    = ESC_ERROR;
  LONG  lResult = 0;
  DWORD dwReturnedDataLength;

  DBG_TRACE_IN(DBG_LVL_SUBENTRY,lQueryEtVersionInfo);

  ASSERTDD(NULL!=ppdev,        "");
  ASSERTDD(NULL!=pVersionInfo, "");

  lResult = (LONG)EngDeviceIoControl( ppdev->hDriver,
                                      IOCTL_VIDEO_GET_ET_VERSION_INFO,
                                      NULL,0,
                                      (VOID*)pVersionInfo,sizeof(ET_VERSION_INFO),
                                      &dwReturnedDataLength);

  if (NO_ERROR==lResult)
  {  
    ASSERT(pVersionInfo->dwSize==sizeof(ET_VERSION_INFO));
    ASSERT(pVersionInfo->dwSize==dwReturnedDataLength);

    lRet = ESC_SUCCESS;
  }

  DBG_TRACE_OUT(DBG_LVL_SUBENTRY,lRet);

  return lRet;
}
#endif

//*******************************************************************************************
static LONG lQueryOpenGlIcdRegPath(
      PPDEV                 ppdev,
  OUT ET_OPENGLICD_REGPATH *pIcdRegPath)
{
  LONG                   lRet   = ESC_ERROR;

  DBG_TRACE_IN(DBG_LVL_SUBENTRY, lQueryOpenGlIcdRegPath);

  ASSERTDD(NULL!=ppdev,       "");
  ASSERTDD(NULL!=pIcdRegPath, "");

  pIcdRegPath->dwSize = sizeof(ET_OPENGLICD_REGPATH);

  szStrCpy(pIcdRegPath->szRegICDKey,  "\\REGISTRY\\Machine\\Software\\Microsoft\\Windows NT\\CurrentVersion\\OpenGLDrivers\\\0");
  szStrCat(pIcdRegPath->szRegICDKey, "RIVATNT");

  // Default String build by environment
  szStrCpy(pIcdRegPath->szRegICDName, "nvoglnt");

  DBG_PRINTS(DBG_LVL_FLOW1, pIcdRegPath->szRegICDKey);
  DBG_PRINTS(DBG_LVL_FLOW1, pIcdRegPath->szRegICDName);

  lRet = ESC_SUCCESS;

  DBG_TRACE_OUT(DBG_LVL_SUBENTRY, lRet);

  return lRet;
}



#define GAMMA_TABLE_ENTRIES (256 * 3)

// CSchalle 8/25/99 2:21:09 PM: new
static LONG lEscET_EDD_GETGAMMARAMP(IN const  PPDEV                 ppdev, 
                                    IN const  ET_QUERY*             pET_Query, 
                                    OUT       PET_EDD_GETGAMMARAMP  pETGetGammaRamp)
{
  LONG lRet;

  DBG_TRACE_IN(DBG_LVL_SUBENTRY, lEscET_EDD_GETGAMMARAMP);

  ASSERT(ppdev            != NULL);
  ASSERT(pET_Query        != NULL);
  ASSERT(pETGetGammaRamp  != NULL);

  if(nvGetGammaRamp(ppdev, &pETGetGammaRamp->red[0]) == 256)
  {
    pETGetGammaRamp->dwResult = ET_VALUE_OK;
    pETGetGammaRamp->dwSize   = sizeof(ET_EDD_GETGAMMARAMP);
    lRet                      = ESC_SUCCESS;
  }
  else
  {
    pETGetGammaRamp->dwResult = ET_VALUE_BLOCKED;
    pETGetGammaRamp->dwSize   = 0;
    lRet                      = ESC_ERROR;
  }

  DBG_TRACE_OUT(DBG_LVL_SUBENTRY, lRet);
  return lRet;
}


// CSchalle 8/25/99 2:08:44 PM: new
static LONG lEscET_EDD_SETGAMMARAMP(IN        PPDEV ppdev, 
                                    IN const  PET_EDD_SETGAMMARAMP  pETSetGammaRamp, 
                                    OUT       PET_SET_VALUE_RET     pETSetValueRet)
{
  LONG lRet;

  DBG_TRACE_IN(DBG_LVL_SUBENTRY, lEscET_EDD_SETGAMMARAMP);

  ASSERT(ppdev           != NULL);
  ASSERT(pETSetGammaRamp != NULL);
  ASSERT(pETSetValueRet  != NULL);

  // prepare output structure
  RtlZeroMemory(pETSetValueRet, sizeof(ET_SET_VALUE_RET));
  pETSetValueRet->dwSize  = sizeof(ET_SET_VALUE_RET);

  if(nvSetGammaRamp(ppdev, &pETSetGammaRamp->red[0]) == 256)
  {
    pETSetValueRet->dwFlags = ET_VALUE_OK;
    lRet                    = ESC_SUCCESS;
  }
  else
  {
    pETSetValueRet->dwFlags = ET_VALUE_BLOCKED;
    lRet                    = ESC_ERROR;
  }

  DBG_TRACE_OUT(DBG_LVL_SUBENTRY, lRet);
  return lRet;
}

// CSchalle 8/26/99 3:17:22 PM:new
// FNicklis 05.10.1999: Allow all IDs and not only ET_VALUE_BIN_GDI_GAMMARAMP
static LONG lEscET_SET_VALUE_BINARY(IN        PPDEV ppdev, 
                                    IN const  PET_SET_VALUE_BINARY  pETSetValueBinary, 
                                    OUT       PET_SET_VALUE_RET     pETSetValueRet)
{
  LONG  lRet = ESC_ERROR;
  LONG  lSize;

  ASSERT(ppdev              != NULL);
  ASSERT(pETSetValueBinary  != NULL);
  ASSERT(pETSetValueRet     != NULL);

  DBG_TRACE_IN(DBG_LVL_SUBENTRY, lEscET_SET_VALUE_BINARY);

  // prepare output structure
  pETSetValueRet->dwID    = pETSetValueBinary->dwID;
  pETSetValueRet->dwSize  = sizeof(ET_SET_VALUE_RET);
  pETSetValueRet->dwFlags = ET_VALUE_UNKNOWN;

  // calculate amount of data behind dwData
  lSize = pETSetValueBinary->hdr.dwSize - ET_SET_VALUE_BINARY_HDR_SIZE;

  switch(pETSetValueBinary->dwID)
  {
    case ET_VALUE_BIN_GDI_GAMMARAMP:
    {

      __try // never trust...
      {
        ASSERT(lSize == GAMMA_TABLE_ENTRIES * sizeof(USHORT));

        // refuse to store partial table, or in palettized modes!
        if(lSize                == GAMMA_TABLE_ENTRIES * sizeof(USHORT) && 
           ppdev->iBitmapFormat >  BMF_8BPP)
        {
          if(REG_OK == lRegSetIdBinary(ppdev->hDriver, 
                                       ET_VALUE_BIN_GDI_GAMMARAMP, 
                                       (const BYTE *)pETSetValueBinary->ajData, 
                                       lSize))
          {
            DISPDBG((DBG_LVL_ERROR, "storing gamma table to registry..."));

            pETSetValueRet->dwFlags = ET_VALUE_OK;
      
            lRet                    = ESC_SUCCESS;
          }
        }
        else
        {
          DISPDBG((DBG_LVL_ERROR, "refusing to store gamma table of size %d at BMF %d", lSize, ppdev->iBitmapFormat));
        }
      }
      __except(EXCEPTION_EXECUTE_HANDLER)
      {
        DISPDBG((DBG_LVL_ERROR, "ERROR in lEscET_SET_VALUE_BINARY: Exception failed with: %s",szDbg_GetExceptionCode(GetExceptionCode())));
      }
      break;
    }

    default: 
    {
      // write lSize Bytes to registry key with dwID
      if(REG_OK == lRegSetIdBinary(ppdev->hDriver, 
                                   pETSetValueBinary->dwID, 
                                   (const BYTE *)pETSetValueBinary->ajData, 
                                   lSize))
      {
        // succeeded 
        pETSetValueRet->dwFlags = ET_VALUE_OK;      
        lRet                    = ESC_SUCCESS;
      }
      break;
    }
  }
  
  DBG_TRACE_OUT(DBG_LVL_SUBENTRY, lRet);

  return lRet;
}


/*
** lEscET_DPMS
**
** handles ET_DPMS ELSA escape.
**
** return: ESC_SUCCESS:      ok, returned data valid
**         ESC_ERROR:        failed, returned data invalid
**
** FNicklis 28.09.1999: New
** FNicklis 06.10.1999: Use IOCTL to Miniport
*/
static LONG lEscET_DPMS(
  IN     PPDEV        ppdev, 
  IN     ET_DPMS_IN  *pIn, 
     OUT ET_DPMS_OUT *pOut)
{
  LONG  lRet    = ESC_ERROR;
  LONG  lResult = 0;
  DWORD dwReturnedDataLength;


  DBG_TRACE_IN(DBG_LVL_SUBENTRY_NLF, lEscET_DPMS);
  DISPDBG((DBG_LVL_SUBENTRY_NPLF, " pIn:0x%x, pOut:0x%x",pIn,pOut));

  ASSERT(NULL!=ppdev);
  ASSERT(NULL!=pIn);

#if 0 // FNicklisch 14.09.00: not supported yet
    lResult = (LONG)EngDeviceIoControl( ppdev->hDriver,
                                      IOCTL_VIDEO_ET_DPMS,
                                      (VOID*)pIn, sizeof(ET_DPMS_IN),
                                      (VOID*)pOut,sizeof(ET_DPMS_OUT),
                                      &dwReturnedDataLength);

  if (NO_ERROR==lResult)
  {  
    ASSERT(pOut->dwSize==sizeof(ET_DPMS_OUT));
    ASSERT(pOut->dwSize==dwReturnedDataLength);

    // remember set mode!
    ppdev->pElsaPDEV->lDPMSMode = pOut->lMode;

    lRet = ESC_SUCCESS;
  }

#endif // FNicklisch 14.09.00: not supported yet

  DBG_TRACE_OUT(DBG_LVL_SUBENTRY,lRet);

  return lRet;
}

/*
** lEscET_QUERY_TEMPERATURE
**
** handles ET_QUERY_TEMPERATUR ELSA escape.
**
** return: ESC_SUCCESS:      ok, returned data valid
**         ESC_ERROR:        failed, returned data invalid
**
** FNicklis 30.09.1999: New
*/
static LONG lEscET_QUERY_TEMPERATURE(
  IN    PPDEV                 ppdev, 
  IN    ET_QUERY             *petQuery, 
    OUT ET_QUERY_TEMPERATURE *petTemperature)
{
  LONG lRet=ESC_ERROR;

  DBG_TRACE_IN(DBG_LVL_SUBENTRY_NLF, lEscET_QUERY_TEMPERATURE);
#if 0 // ELSA-FNicklis 04.07.00: unused
    DISPDBG((DBG_LVL_SUBENTRY_NPLF, " petQuery:0x%x, petTemperature:0x%x",petQuery,petTemperature));

  ASSERT(NULL!=ppdev);
  ASSERT(NULL!=petQuery);
  ASSERT(NULL!=petTemperature);

  petTemperature->dwSize              = sizeof(ET_QUERY_TEMPERATURE);

  // set capabilities of driver e.g. ET_TEMPERATURE_FANSLOW | ET_TEMPERATURE_OVERTEMP
  petTemperature->dwTemperatureCaps   = 0;                          

  // set error status: e.g. ET_TEMPERATURE_FANSLOW if fan irq occured
  // get this from miniport, where irq handler receives fan irqs
  petTemperature->dwTemperatureStatus = (DWORD) flGetTemperatureStatus(ppdev->pElsaPDEV->pHDE); 

  // unused for now
  petTemperature->dwTemperatureValue  = 0;                          

  lRet=ESC_SUCCESS;
#endif // ELSA-FNicklis 04.07.00: unused

  DBG_TRACE_OUT(DBG_LVL_SUBENTRY, lRet);
  return (lRet);
}
        
#endif //RUN_WITH_NVIDIA_MINIPORT 
#endif //USE_WKS_ELSA_TOOLS
// End of wkselsatoolsescape.c
