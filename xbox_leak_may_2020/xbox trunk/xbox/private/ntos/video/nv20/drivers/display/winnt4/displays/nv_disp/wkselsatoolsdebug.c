//*****************************Module*Header******************************
//
// Module Name: wkselsatoolsdebug.c
//
// Debug code 
//
// FNicklisch 09/14/2000: New, derived from debug.c
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
#include "ELSAESC.h"
#include "wkselsatoolsescape.h"
#include "wkselsatoolstoolbox.h"
#include "wkselsatoolsregistry.h"

#if DBG
// export header file
#include <wkselsatoolsdebug.h>


// FNicklis 21.10.98: New sub escapes defined
DBG_NAMEINDEX aDbgETSubEscName[]=
{
  // Sub escapes, common for Windows 95 and Windows NT:
  DBG_MAKE_NAMEINDEX(SUBESC_ET_QUERY_VERSION_INFO     ),

  DBG_MAKE_NAMEINDEX(SUBESC_ET_QUERY_VALUE_STRING     ), 
  DBG_MAKE_NAMEINDEX(SUBESC_ET_QUERY_VALUE_DWORD      ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_QUERY_VALUE_BINARY     ),

  DBG_MAKE_NAMEINDEX(SUBESC_ET_SET_VALUE_STRING       ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_SET_VALUE_DWORD        ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_SET_VALUE_BINARY       ),

  // 0x30-0x4F reserved for WINman
  DBG_MAKE_NAMEINDEX(SUBESC_ET_WINMAN_DATA_GET        ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_WINMAN_DATA_SET        ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_WINMAN_DDC_EDID_GET    ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_WINMAN2_DATA_GET       ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_WINMAN2_DATA_SET       ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_WINMAN2_DDC_EDID_GET   ),

  // 0x50-0x6F reserved for elsa directdraw caps
  DBG_MAKE_NAMEINDEX(SUBESC_ET_EDD_GETCAPS            ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_EDD_GETFEATURES        ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_EDD_SETFEATURES        ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_EDD_GETVIDMEMINFO      ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_EDD_GETCOLORADJUSTMENT ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_EDD_SETCOLORADJUSTMENT ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_EDD_SETGAMMARAMP       ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_EDD_GETGAMMARAMP       ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_EDD_GETCOLORCAPSLIMITS ),

  // 0x100-0x11F reserved for Video-In / -Out Escapes
  //
  DBG_MAKE_NAMEINDEX(SUBESC_ET_VIDEO_DMACOPYSM        ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_VIDEO_OUTCONTROL       ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_RMAPI_CONFIGSETGET     ),

  // 0x200-0x21F reserved for all tools to decide if they are
  //             allowed to pop up, only to be used in combination
  //             with a resctricted call RES_ESC_ELSA_TOOL
  //
  DBG_MAKE_NAMEINDEX(SUBESC_STEREO_TOOL               ),

  // 0x300-0x3FF reserved for hardware status and control
  //
  DBG_MAKE_NAMEINDEX(SUBESC_ET_QUERY_TEMPERATURE      ),
  
  
  // ********************************************************
  // Windows 95 sub escapes:

  DBG_MAKE_NAMEINDEX(SUBESC_ET_QUERY_MONITORKEY_REGPATH    ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_QUERY_D3DSETTINGS_REGPATH   ),

  // ********************************************************
  // Windows NT sub escapes:

  DBG_MAKE_NAMEINDEX(SUBESC_ET_QUERY_OPENGLICD_REGPATH),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_DUO_ESC_GETPANELINFO   ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_DUO_ESC_SETPANELINFO   ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_POWERLIB_START         ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_POWERLIB_EXIT          ),

  // 0x10-0x2F reserved for driver internal escapes
  DBG_MAKE_NAMEINDEX(SUBESC_ET_GETREGISTER            ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_SETREGISTER            ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_MODIFYREGISTER         ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_SCROLL                 ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_GFXOPTIONQUERY         ),

  DBG_MAKE_NAMEINDEX(SUBESC_ET_NOTIFYDRIVER           ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_SAA                    ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_DDSURFOVERRIDE         ),

  DBG_MAKE_NAMEINDEX(SUBESC_ET_MULTIBOARDSETUP        ),

  DBG_MAKE_NAMEINDEX(SUBESC_ET_DPMS                   ),

  // 0x30- ... Common driver Escapes
  DBG_MAKE_NAMEINDEX(SUBESC_ET_SETWINDOW_STEREOMODE   ),
  DBG_MAKE_NAMEINDEX(SUBESC_ET_QUERY_ICDCLIENTINFO    ),
};

char *szDbg_GetETSubEscName(LONG SubEscNr)
{
  return (DBG_GETDBG_NAMEINDEX(aDbgETSubEscName, SubEscNr));
}

// 17.09.1998 FNicklis: New
char *pszDbg_GetFLOH(FLOH floh)
{
  static char pszRet[80];

  pszRet[0]='\0'; // delete string 

  if (0!=floh)
  {
    strcpy( pszRet, "(");

    if (floh & FLOH_ONLY_IF_ROOM) 
    {  // Don't kick stuff out of offscreen memory to make room
      strcat( pszRet, "FLOH_ONLY_IF_ROOM");
      floh &= ~FLOH_ONLY_IF_ROOM;
      if (floh)
        strcat( pszRet, "+");
    }

    if (floh & FLOH_MAKE_PERMANENT) 
    { // Allocate a permanent entry
      strcat( pszRet, "FLOH_MAKE_PERMANENT");
      floh &= ~FLOH_MAKE_PERMANENT;
      if (floh)
        strcat( pszRet, "+");
    }

    if (floh & FLOH_RESERVE) // Allocate an off-screen entry,
    {
      strcat( pszRet, "FLOH_RESERVE");
      floh &= ~FLOH_RESERVE;
      if (floh)
        strcat( pszRet, "+");
    }

    if (floh)
    {
      strcat( pszRet, "unknown");
    }

    strcat( pszRet, ")");
  }
  return (pszRet);
}

// FN new, taken from old glint sources

VOID vDbg_PrintPOINTL(FLONG flFlags, PCHAR pcString, const POINTL *pptl)
{
  if (pptl)
  {
    DISPDBG((flFlags, "%s: x:%-5d y:%-5d",pcString, pptl->x, pptl->y));
  }
  else
  {
    DISPDBG((flFlags, "%s: NULL", pcString));
  }
}

VOID vDbg_PrintRECTL(FLONG flFlags, PCHAR pcString, const RECTL *prcl)
{
  if (prcl)
  {
    DISPDBG((flFlags, "%s: l:%-5d t:%-5d r:%-5d b:%-5d (w:%-5d h:%-5d)", pcString,
      prcl->left, prcl->top, prcl->right, prcl->bottom, prcl->right-prcl->left,prcl->bottom-prcl->top));
  }
  else
  {
    DISPDBG((flFlags, "%s: NULL", pcString));
  }
}


VOID vDbg_PrintSIZEL(FLONG flFlags, PCHAR pcString, const SIZEL *psiz)
{
  if (psiz)
  {
    DISPDBG((flFlags, "%s: cx:%08x cy:%08x",
      pcString, psiz->cx, psiz->cy));
  }
  else
  {
    DISPDBG((flFlags, "%s: NULL", pcString));
  }
}

VOID vDbg_PrintCLIPOBJiDComplexity(FLONG flFlags, BYTE iDComplexity)
{
  switch(iDComplexity)
  {
    case DC_TRIVIAL:
      DISPDBG((flFlags, " DC_TRIVIAL"));
      break;
    case DC_RECT:   
      DISPDBG((flFlags, " DC_RECT"));
      break;
    case DC_COMPLEX:
      DISPDBG((flFlags, " DC_COMPLEX"));
      break;
    default: 
      DISPDBG((flFlags, " DC_??:0x%x",iDComplexity));
      break;
  }
}

VOID vDbg_PrintCLIPOBJiFComplexity(FLONG flFlags, BYTE iFComplexity)
{
  switch(iFComplexity)
  {
    case FC_RECT:    
      DISPDBG((flFlags, "FC_RECT"));
      break;
    case FC_RECT4:   
      DISPDBG((flFlags, "FC_RECT4"));
      break;
    case FC_COMPLEX: 
      DISPDBG((flFlags, "FC_COMPLEX"));
      break;
    default: 
      DISPDBG((flFlags, "FC_??:0x%x",iFComplexity));
      break;
  }
}

VOID vDbg_PrintCLIPOBJ(FLONG flFlags, PCHAR pcString, const CLIPOBJ *pco)
{
  if (pco)
  {
    DISPDBG((flFlags|DBG_LVL_NOLINEFEED, "%s: 0x%p, iUniq:%08x ", pcString, pco, pco->iUniq));

    vDbg_PrintCLIPOBJiDComplexity(flFlags|DBG_LVL_NOLINEFEED|DBG_LVL_NOPREFIX, pco->iDComplexity);
    DISPDBG((flFlags|DBG_LVL_NOLINEFEED|DBG_LVL_NOPREFIX, " "));
    vDbg_PrintCLIPOBJiFComplexity(flFlags|DBG_LVL_NOPREFIX, pco->iFComplexity);

    DISPDBG((flFlags|DBG_LVL_NOLINEFEED, "%s: ", pcString));
    vDbg_PrintRECTL(flFlags|DBG_LVL_NOPREFIX, "rclBounds", &pco->rclBounds);
  }
  else
  {
    DISPDBG((flFlags, "%s: NULL", pcString));
  }
}

#ifndef PAL_DC
// NT 3.51
#define PAL_DC		      0x00000010
#define PAL_FIXED	      0x00000020
#define PAL_FREE	      0x00000040
#define PAL_MANAGED	    0x00000080
#define PAL_NOSTATIC	  0x00000100
#define PAL_MONOCHROME	0x00000200
#define PAL_DITHER	    0x00000400
#endif

DBG_NAMEINDEX aDbgXLATEOBJiTypeNames[] =
{
  DBG_MAKE_NAMEINDEX(PAL_DC        ),
  DBG_MAKE_NAMEINDEX(PAL_FIXED     ),
  DBG_MAKE_NAMEINDEX(PAL_FREE      ),
  DBG_MAKE_NAMEINDEX(PAL_MANAGED   ),
  DBG_MAKE_NAMEINDEX(PAL_NOSTATIC  ),
  DBG_MAKE_NAMEINDEX(PAL_MONOCHROME),
  DBG_MAKE_NAMEINDEX(PAL_DITHER	   ),
  DBG_MAKE_NAMEINDEX(PAL_INDEXED   ),
  DBG_MAKE_NAMEINDEX(PAL_BITFIELDS ),
  DBG_MAKE_NAMEINDEX(PAL_RGB       ),
  DBG_MAKE_NAMEINDEX(PAL_BGR       ),
};

/*
** Find string matching ulIndex out of DBG_NAMEINDEX array pNameIndex
*/
char *szDbg_GetDBG_NAMEINDEX(IN const DBG_NAMEINDEX *pNameIndex, IN LONG lEntries, IN ULONG ulIndex)
{
  char *szRet=NULL;

  while (lEntries)
  {
    if (pNameIndex->ulIndex == ulIndex)
    { 
      szRet=pNameIndex->szName;
      break;
    }
    pNameIndex++;
    lEntries--;
  }
  
  if (szRet==NULL)
    szRet="unknown\0??????????????????????"; // Added space to allow strange things on the pointer returned!

  return (szRet);
}

char *szDbg_GetXLATEOBJiTypeName(ULONG fl)
{
  return (szDbg_GetDBG_NAMEINDEX(aDbgXLATEOBJiTypeNames, sizeof(aDbgXLATEOBJiTypeNames)/sizeof(aDbgXLATEOBJiTypeNames[0]), fl));
}

VOID vDbg_PrintXLATEOBJiType(FLONG flFlags, USHORT iType)
{
  vDbg_PrintflCapabilities(
    flFlags, 
    "", 
    szDbg_GetXLATEOBJiTypeName,
    (ULONG)iType);
}

VOID vDbg_PrintXLATEOBJflXlate(FLONG flFlags, FLONG flXlate)
{
  if (flXlate & XO_TRIVIAL) 
    DISPDBG( (flFlags, " XO_TRIVIAL"));
  if (flXlate & XO_TABLE) 
    DISPDBG( (flFlags, " XO_TABLE  "));
  if (flXlate & XO_TO_MONO) 
    DISPDBG( (flFlags, " XO_TO_MONO")); 
  if (flXlate & 0x00000008) 
    DISPDBG( (flFlags, " XO_FROM_CMYK")); 
  if (flXlate & 0x00000010) 
    DISPDBG( (flFlags, " XO_DEVICE_ICM")); 
  if (flXlate & 0x00000020) 
    DISPDBG( (flFlags, " XO_HOST_ICM")); 

#if (_WIN32_WINNT >= 0x500)
  #if ((XO_FROM_CMYK != 0x00000008) || (XO_DEVICE_ICM != 0x00000010) || (XO_HOST_ICM != 0x00000020))
    #error Check DDK defintions for XLATES flags!
  #endif
#endif

  if (!(flXlate & (XO_TO_MONO | XO_TABLE | XO_TRIVIAL | 0x00000008 | 0x00000010 | 0x00000020))) 
    DISPDBG( (flFlags, " XO_??? (0x%x)", flXlate));
}

VOID vDbg_PrintXLATEOBJ(FLONG flFlags, PCHAR pcString, const XLATEOBJ *pxlo)
{
  if (pxlo)
  {
    DISPDBG((flFlags, "%s: 0x%p", pcString, pxlo));
    DISPDBG((flFlags, "%s: iUniq:0x%x, cEntries:%d, pulXlate:0x%p", pcString, pxlo->iUniq, pxlo->cEntries, pxlo->pulXlate));

    DISPDBG((flFlags|DBG_LVL_NOLINEFEED, "%s: flXlate:0x%x ", pcString, pxlo->flXlate));
    vDbg_PrintXLATEOBJflXlate(flFlags|DBG_LVL_NOPREFIX, pxlo->flXlate);
    
    DISPDBG((flFlags|DBG_LVL_NOLINEFEED, "%s: iSrcType:0x%x ", pcString, pxlo->iSrcType));
    vDbg_PrintXLATEOBJiType(flFlags|DBG_LVL_NOPREFIX, pxlo->iSrcType);

    DISPDBG((flFlags|DBG_LVL_NOLINEFEED, "%s: iDstType:0x%x ", pcString, pxlo->iDstType));
    vDbg_PrintXLATEOBJiType(flFlags|DBG_LVL_NOPREFIX, pxlo->iDstType);
  }
  else
  {
    DISPDBG((flFlags, "%s: NULL", pcString));   
  }
  DISPDBG((flFlags, "."));   
}

VOID vDbg_PrintSURFOBJiType(FLONG flFlags, USHORT  iType)
{
  switch(iType)
  {
    case STYPE_BITMAP:    DISPDBG( (flFlags," STYPE_BITMAP")); 
                          break;
    case STYPE_DEVBITMAP: DISPDBG( (flFlags, " STYPE_DEVBITMAP")); 
                          break;
    case STYPE_DEVICE:    DISPDBG( (flFlags," STYPE_DEVICE")); 
                          break;
    default:              DISPDBG( (flFlags," STYPE_? 0x%x",iType)); 
                          break;
  }
}

VOID vDbg_PrintSURFOBJiBitmapFormat(FLONG flFlags, ULONG iBitmapFormat)
{
  switch (iBitmapFormat)
  {
    case BMF_8RLE:    DISPDBG((flFlags, " BMF_8RLE" )); break;
    case BMF_4RLE:    DISPDBG((flFlags, " BMF_4RLE" )); break;
    case BMF_32BPP:   DISPDBG((flFlags, " BMF_32BPP")); break;
    case BMF_24BPP:   DISPDBG((flFlags, " BMF_24BPP")); break;
    case BMF_16BPP:   DISPDBG((flFlags, " BMF_16BPP")); break;
    case BMF_8BPP:    DISPDBG((flFlags, " BMF_8BPP" )); break;
    case BMF_4BPP:    DISPDBG((flFlags, " BMF_4BPP" )); break;
    case BMF_1BPP:    DISPDBG((flFlags, " BMF_1BPP" )); break;
    default:          DISPDBG((flFlags, " BMF_?:0x%x",iBitmapFormat));break;
  }
}

VOID vDbg_PrintSURFOBJ(FLONG flFlags, PCHAR pcString, const SURFOBJ *pso)
{
  //char pjPlatz[100];

  if (pso)
  {
    DISPDBG((flFlags|DBG_LVL_NOLINEFEED,"%s: 0x%p ", pcString, pso));   
    vDbg_PrintSURFOBJiType(flFlags|DBG_LVL_NOPREFIX|DBG_LVL_NOLINEFEED, pso->iType);
    DISPDBG((flFlags|DBG_LVL_NOPREFIX|DBG_LVL_NOLINEFEED," "));
    vDbg_PrintSURFOBJiBitmapFormat(flFlags|DBG_LVL_NOPREFIX, pso->iBitmapFormat);

    DISPDBG((flFlags,"%s: dhsurf:0x%p, hsurf:0x%p, phpdev:0x%p, hdev:0x%p", pcString,
             pso->dhsurf, pso->hsurf, pso->dhpdev, pso->hdev)); 

    DISPDBG((flFlags,"%s: sizlBitmap: cx:%d, cy:%d, cjBits:0x%x, pvBits:0x%p", pcString,
          pso->sizlBitmap.cx,pso->sizlBitmap.cy, pso->cjBits,pso->pvBits));

    DISPDBG((flFlags,"%s: pvScan0:0x%p, lDelta:0x%x, iUniq:0x%x, fjBmp:%08x", pcString,
          pso->pvScan0,pso->lDelta, pso->iUniq, pso->fjBitmap));    
  }
  else
  {
    DISPDBG((flFlags,"%s: NULL", pcString));   
  }
}

/*
** DBG_TRACE_DUMP
** 
** prints the trace dump of all known functions on the stack
** 
** FNicklis 02.10.1998: New
*/

VOID vDbg_PrintTRACE_DUMP(FLONG flDbgLvl)
{
#if 0 // FNicklisch 14.09.00: unused
  DISPDBG((flDbgLvl,"DBG_TRACE_DUMP printing call stack"));
  if (lDbgUnhandledFnStack>0)
  {
    DISPDBG((flDbgLvl,"  %3d-%d (%d) calls not catched",lDbgUnhandledFnStack+lDbgFnStack,lDbgFnStack,lDbgUnhandledFnStack));
  }
  else
  {
    LONG l;
    for (l=lDbgFnStack-1; l>=0; l--)
      DISPDBG((flDbgLvl,"  %3d: %s at 0x%p",l,aDbgFnStack[l].szName,aDbgFnStack[l].pfn));
  }
#endif // FNicklisch 14.09.00: unused
}

char *szDbg_TRACE_GetCurrentFn(VOID)
{
#if 0 // FNicklisch 14.09.00: unused
  static LONG lCount=0;
  if ( (lDbgFnStack<1) || (lDbgUnhandledFnStack>0) )
    return ("unknown");

  if (   (lCount<20)
      && (NULL==aDbgFnStack[lDbgFnStack-1].szName) )
  {
    DISPDBG((DBG_LVL_ERROR, "ERROR in szDbg_TRACE_GetCurrentFn: corrupt function stack..."));
    vDbg_PrintTRACE_DUMP(DBG_LVL_ERROR);
    lCount++;
  }

  return (aDbgFnStack[lDbgFnStack-1].szName);
#else
  return ("unknown");
#endif // FNicklisch 14.09.00: unused
}


/*
** define an array with all RegEscapeIDs
*/
DBG_NAMEINDEX aDbgExceptionCode[] =
{
  DBG_MAKE_NAMEINDEX(STATUS_WAIT_0                  ),
  DBG_MAKE_NAMEINDEX(STATUS_ABANDONED_WAIT_0        ),
  DBG_MAKE_NAMEINDEX(STATUS_USER_APC                ),
  DBG_MAKE_NAMEINDEX(STATUS_TIMEOUT                 ),
  DBG_MAKE_NAMEINDEX(STATUS_PENDING                 ),
  DBG_MAKE_NAMEINDEX(STATUS_SEGMENT_NOTIFICATION    ),
  DBG_MAKE_NAMEINDEX(STATUS_GUARD_PAGE_VIOLATION    ),
  DBG_MAKE_NAMEINDEX(STATUS_DATATYPE_MISALIGNMENT   ),
  DBG_MAKE_NAMEINDEX(STATUS_BREAKPOINT              ),
  DBG_MAKE_NAMEINDEX(STATUS_SINGLE_STEP             ),
  DBG_MAKE_NAMEINDEX(STATUS_ACCESS_VIOLATION        ),
  DBG_MAKE_NAMEINDEX(STATUS_IN_PAGE_ERROR           ),
  DBG_MAKE_NAMEINDEX(STATUS_INVALID_HANDLE          ),
  DBG_MAKE_NAMEINDEX(STATUS_NO_MEMORY               ),
  DBG_MAKE_NAMEINDEX(STATUS_ILLEGAL_INSTRUCTION     ),
  DBG_MAKE_NAMEINDEX(STATUS_NONCONTINUABLE_EXCEPTION),
  DBG_MAKE_NAMEINDEX(STATUS_INVALID_DISPOSITION     ),
  DBG_MAKE_NAMEINDEX(STATUS_ARRAY_BOUNDS_EXCEEDED   ),
  DBG_MAKE_NAMEINDEX(STATUS_FLOAT_DENORMAL_OPERAND  ),
  DBG_MAKE_NAMEINDEX(STATUS_FLOAT_DIVIDE_BY_ZERO    ),
  DBG_MAKE_NAMEINDEX(STATUS_FLOAT_INEXACT_RESULT    ),
  DBG_MAKE_NAMEINDEX(STATUS_FLOAT_INVALID_OPERATION ),
  DBG_MAKE_NAMEINDEX(STATUS_FLOAT_OVERFLOW          ),
  DBG_MAKE_NAMEINDEX(STATUS_FLOAT_STACK_CHECK       ),
  DBG_MAKE_NAMEINDEX(STATUS_FLOAT_UNDERFLOW         ),
  DBG_MAKE_NAMEINDEX(STATUS_INTEGER_DIVIDE_BY_ZERO  ),
  DBG_MAKE_NAMEINDEX(STATUS_INTEGER_OVERFLOW        ),
  DBG_MAKE_NAMEINDEX(STATUS_PRIVILEGED_INSTRUCTION  ),
  DBG_MAKE_NAMEINDEX(STATUS_STACK_OVERFLOW          ),
  DBG_MAKE_NAMEINDEX(STATUS_CONTROL_C_EXIT          ),
};

char *szDbg_GetExceptionCode(DWORD dwID)
{
  return (szDbg_GetDBG_NAMEINDEX(aDbgExceptionCode, sizeof(aDbgExceptionCode)/sizeof(aDbgExceptionCode[0]), (LONG)dwID));
}

DBG_NAMEINDEX aDbgET_SET_VALUE_RETdwFlags[] =
{
  DBG_MAKE_NAMEINDEX(ET_VALUE_OK     ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_REBOOT ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DEFAULT),
  DBG_MAKE_NAMEINDEX(ET_VALUE_BLOCKED),
  DBG_MAKE_NAMEINDEX(ET_VALUE_ERROR	),
  DBG_MAKE_NAMEINDEX(ET_VALUE_UNKNOWN),
};

// get single string out of aDbgET_SET_VALUE_RETdwFlags matching dwFlags
char *szDbg_GetET_SET_VALUE_RETdwFlags(DWORD dwFlags)
{
  return (szDbg_GetDBG_NAMEINDEX(aDbgET_SET_VALUE_RETdwFlags, sizeof(aDbgET_SET_VALUE_RETdwFlags)/sizeof(aDbgET_SET_VALUE_RETdwFlags[0]), dwFlags));
}

// get complete description to dwFlags 
VOID vDbg_PrintET_VALUE_RETdwFlags(
  FLONG flDbgLvl, 
  PCHAR pcString, 
  DWORD dwFlags)
{
  vDbg_PrintflCapabilities(
    flDbgLvl, 
    pcString, 
    szDbg_GetET_SET_VALUE_RETdwFlags,
    dwFlags);
}

DBG_NAMEINDEX aDbgESC_RETURN[] =
{
  DBG_MAKE_NAMEINDEX(ESC_SUCCESS     ),
  DBG_MAKE_NAMEINDEX(ESC_NOTSUPPORTED),
  DBG_MAKE_NAMEINDEX(ESC_ERROR       ),
};

// get single string out of aDbgESC_RETURN matching dwFlags
char *szDbg_GetESC_RETURN(LONG lRet)
{
  return (szDbg_GetDBG_NAMEINDEX(aDbgESC_RETURN, sizeof(aDbgESC_RETURN)/sizeof(aDbgESC_RETURN[0]), (ULONG)lRet));
}

DBG_NAMEINDEX aDbgREG_RETURN[] =
{
  DBG_MAKE_NAMEINDEX(REG_ERROR  ),
  DBG_MAKE_NAMEINDEX(REG_OK     ),
  DBG_MAKE_NAMEINDEX(REG_DEFAULT),
};

// get single string out of aDbgREG_RETURN matching dwFlags
char *szDbg_GetREG_RETURN(LONG lRet)
{
  return (szDbg_GetDBG_NAMEINDEX(aDbgREG_RETURN, sizeof(aDbgREG_RETURN)/sizeof(aDbgREG_RETURN[0]), (ULONG)lRet));
}


/*
** define an array with all RegEscapeIDs
*/
DBG_NAMEINDEX aDbgET_VALUE_IDs[] =
{
  // Windows 9x
  //
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_D3DOGL_MCLK                             ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_D3DOGL_NVCLK                            ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_D3DOGL_OVERCLOCK_ENABLE                 ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_D3DOGL_OVERCLOCK_CRASHSTATUS            ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_D3DOGL_DEFAULT_MCLK                     ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_D3DOGL_DEFAULT_NVCLK                    ),

  // Windows NT
  //
  // Basic/system 0x20000000-0x200000ff
  //#define ET_VALUE_SZ_IMAGEPATH                               0x20000000
  //#define ET_VALUE_SZ_INSTALLEDDISPLAYDRIVERS                 0x20000001

  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_DESKTOPONMAXIMIZE                   ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_ENABLEDEVICEBITMAPS                 ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_ENABLEPCIDISCONNECT                 ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_EXPORTONLYDOUBLEBUFFERMODES         ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_NTEAMPRIVATEKEY                     ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_P6ENABLEUSWC                        ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_REVERSESCREENORDER                  ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_USEGETACCESSRANGES                  ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_USELCDPANEL                         ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_VERTICALSCREENORDER                 ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_WAITFORFIFOS                        ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_DEVICEORIGINX                       ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_DEVICEORIGINY                       ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_DEVICESIZEX                         ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_DEVICESIZEY                         ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_VISUALORIGINX                       ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_VISUALORIGINY                       ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_VISUALSIZEX                         ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_VISUALSIZEY                         ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_DELLXXL                             ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_PERMEDIACLOCKSPEED                  ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_GLINTCLOCKSPEED                     ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_GLINTGAMMACLOCKSPEED                ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_GLINTDELTACLOCKSPEED                ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_HWLINES                             ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_FIFOEXTRA_P2            ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_FIFOEXTRA               ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_FIFOUSAGE               ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_EXPORTMULTIBOARDMODES   ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_SURFACESONBANKBOUNDARIES            ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_DMABUFFERSIZE                       ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_DMAPADDING                          ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_BOARD_PATTERN                       ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_AGP_DMA                             ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GDI_FRAMEBUFFER_DMA                     ),

  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_MAP_FRAMEBUFFER                         ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_MAP_GLINTMMIO                           ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_MAP_DELTAMMIO                           ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_MAP_LOCALBUFFER                         ),

  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_PPRO_ENABLEWCB                          ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ALPHA_MIATA                             ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_PRINTANDCHECKRENDERSTATE                ),

  DBG_MAKE_NAMEINDEX(ET_VALUE_BIN_GDI_GAMMARAMP                          ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_BIN_OEM_MODELIST                           ),

  // ICDInterface.* 0x20000100-0x200001ff
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_DISABLE                    ),
  //DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_DOUBLEBUFFER               ),
  //DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_MAXDOUBLEBUFFER            ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_NUMBEROFDISPLAYBUFFERS     ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_MAXNUMBEROFDISPLAYBUFFERS  ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_SUPPORTSINGLEBUFFEREDGDI   ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_SUPPORTDOUBLEBUFFEREDGDI   ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_EXPORTALPHA                ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_SUPPORTOVERLAYPLANES       ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_VBLANKWAIT                 ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_PAGEFLIPPING               ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_LBWIDTH                    ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_SIZEOFBUFFER               ),
  
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_DUALTXLBMOVES              ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_EXTENDEDPAGEFLIP           ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_FNSENDDATATST              ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_FULLSCNTOPBORDER           ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_LBSIZE                     ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_P2ENABLEBYPASSDMA          ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_P2ENABLEBYPASSPATCH        ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_PAGEFLIPFORCESYNC          ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_TEXTURESIZE                ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_USESOFTWAREWRITEMASK       ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_USESINGLETX                ),
  
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_EXPORTCOLORINDEX           ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_OPENGLICDNAME              ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_OPENGLICDELSAOGLNAME       ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_PFDREDUCTION               ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_STEREOFORCEBUFFERS         ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_STEREOLEFTEYETOP           ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_STEREOSUPPORT              ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_TEXTURESMARTCACHE          ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_TEXTURETRANSIENTONLY       ),
  
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_MAXTEXTURESIZE             ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_PFD_TYPE_COLORINDEX        ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_LBPATCHED                  ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_DUALGLINTWINDOWMOVES       ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_FORCEGDIBROADCASTINGINOGLVIEWS),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_PFD_STEREO                 ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_PFD_SWAP_LAYER_BUFFERS     ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_PFD_SUPPORT_GDI            ),

  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_FORCE_STENCIL              ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_FORCE_MORETHAN16BITZ       ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_TEXTURE_REDUCE_FACTOR      ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_PFD_SWAP_COPY              ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_ICDINTERFACE_PFD_SWAP_EXCHANGE          ),

  // GLintDMA.* 0x20000200-0x2000024f
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GLINTDMA_NUMBEROFBUFFERS                ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GLINTDMA_NUMBEROFSUBBUFFERS             ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GLINTDMA_SIZEOFBUFFERS                  ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GLINTDMA_CACHEDBUFFERS                  ),
  
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_AGP_ENABLE                              ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GLINTDMA_SUPPORTAGPONBX                 ),
  
  //above...DBG_MAKE_NAMEINDEX(ET_VALUE_DW_AGP_ENABLE                              ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_AGP_RATE                                ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_AGP_SIDEBANDADDRESSING                  ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_AGP_REQUEST_DEPTH                       ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_AGP_FAST_WRITES                         ),
  
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_DMAWAITFORPOLL                          ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_AGP_CACHING                             ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_AGP_MSWORKAROUND                        ),
  

  // GLintIRQ.* 0x20000250-0x200002ff
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_GLINTIRQ_ENABLE                         ),

  // OpenGL.* 0x20000300-0x200003ff
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_USEHIQUALITYTEXTURE              ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_UNIXCONFORM                      ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_DOUBLEWRITE                      ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_DISABLEFASTCLEAR                 ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_DISABLEMIPMAPS                   ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_TEXTURECOMPRESSION               ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_ENABLEP2ANTIALIAS                ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_SUPPORTOPENGLVERSION11           ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_FORCETRUECOLOR16BIT              ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_SUPPORTSOFTIMAGE                 ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_SUPPORTSOFTIMAGE_351             ),
  
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_DRAWLINEENDS                     ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_ENABLEFIFOACCESS                 ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_DISABLEDELTA                     ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_MXDISABLEMIPMAPS                 ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_FORCENNTEXFILTER                 ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_DISABLEDMAFLUSHES                ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_DISABLEGXCULL                    ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_INVERTGXCFORMAT                  ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_PERSPECTIVECORRECTION            ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_REALLOCBLOCKSIZE                 ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_DISABLEDLISTTEXTURES             ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_DELTANODRAW                      ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_ENABLEBYPASSPATCH                ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_ENABLEBYPASSDMA                  ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_FORCENESTEDDMA                   ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_DISABLETRIANGLEPACKET            ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_PRESERVETRUECOLORTEXTURES        ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_DUALTXPRO                        ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_VERTEXCOOPTMAX                   ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_NVVVOPTIMIZATION                 ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_MIPMAPCONTROL                    ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_DISABLETEXTUREPATCHING           ),
  
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_USEGIDCLIPPING                   ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_PLACEMIPMAPSINALTERNATEBANKS     ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_DISABLETEXTUREMANAGERLRU         ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_SUPPORTPRODESIGNER               ),

  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_DMAMAXCOUNT                      ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_DMAMINPUSHCOUNT                  ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_MAXTEXTURESIZE                   ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_MINVIDMEMTEXWIDTH                ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_MAXTEXHEAPSIZE                   ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_LIGHTINGANALYSIS                 ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_LIGHTPICKINGOVERRIDE             ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_STATISTICS                       ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_BENCHING                         ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_USEKATMAI                        ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_VERTEXCACHESIZE                  ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_PIXELTUBE                        ),

  // OpenGL AGP settings
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_AGP_DMA                          ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_AGP_TEXTURES                     ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_FORCE_AGP_TEXTURES               ),

  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_FORCE_CLIPPING                   ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_ERRORLOG                         ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_DUMPLOG                          ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_MCAD_MODE                        ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_SUPPORT_P2_CI4TEXTURES           ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_FORCE_SINGLEBUFFERED             ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_ALIGNED_SURFACES                 ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_FRAMEBUFFER_DMA                  ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_AUTODESKEXTENSIONS               ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_USESENDDATAESCAPE                ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_DISABLE_STENCIL                  ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_DISABLE_ALPHA                    ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_PREFER_AGP_TEXTURES              ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_DMAMINPUSHCOUNT_SENDDATA         ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_OPENGL_MULTIBOARD                       ),

  
    
  // Heidi/Kinetix.* 0x20000400-0x200004ff
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_HEIDI_USEEXCLUSIVE                      ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_HEIDI_USELINEARTEXTUREFILTER            ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_KTX_BUFFERREGIONS                       ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_KTX_SWAPHINTS                           ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_KTX_ELSABUFFERREGIONS                   ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_KTX_BUFFER_REGIONS_FULL                 ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_KTX_BUFFER_REGIONS_FIX                  ),

  // HardwareInformation.* 0x20000500-0x200005ff
  DBG_MAKE_NAMEINDEX(ET_VALUE_SZ_HARDWAREINFORMATION_ADAPTERSTRING       ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_SZ_HARDWAREINFORMATION_CHIPTYPE            ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_SZ_HARDWAREINFORMATION_DACTYPE             ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_HARDWAREINFORMATION_MEMORYSIZE          ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_SZ_BOARDNAME                               ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_SZ_DESCRIPTION                             ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_SZ_SERNUM                                  ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_SZ_HARDWAREINFORMATION_BIOSSTRING          ),

  // DDraw ids    0x20000600-0x200006ff
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_DDRAWMULTIBOARDPRIMARY                  ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_DDRAWENABLEVIDEOIO                      ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_DDRAWMULTIBOARDHW_ENABLE                ),

  // LCD Panel    0x20000700-0x200007ff
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_LCDPANELBORDERLEFTRIGHT                 ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_LCDPANELBORDERTOPBOTTOM                 ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_LCDPANELSTEPLEFTRIGHT                   ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_LCDPANELSTEPTOPBOTTOM                   ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_LCDPANELPANSTARTXRES                    ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_LCDPANELSTRETCHXRES                     ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_LCDPANELSINGLEMODE                      ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_LCDPANELENABLECRT                       ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_LCDPANELENABLELCD                       ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_LCDPANELENABLETV                        ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_LCDPANELTRUECOLORDITHER                 ),
  DBG_MAKE_NAMEINDEX(ET_VALUE_DW_LCDPANELTVFLICKERFILTER                 ),

  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_RESERVED0),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_RESERVED1),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_RESERVED2),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_RESERVED3),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_RESERVED4),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_RESERVED5),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_RESERVED6),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_RESERVED7),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_RESERVED8),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_RESERVED9),
  
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ICDRESERVED0),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ICDRESERVED1),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ICDRESERVED2),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ICDRESERVED3),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ICDRESERVED4),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ICDRESERVED5),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ICDRESERVED6),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ICDRESERVED7),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ICDRESERVED8),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ICDRESERVED9),

  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ERAZOR_II ),

  
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_CORE_CLOCK        ),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_MEMORY_CLOCK      ),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_MEMORY_TIMING     ),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_HW_STANDARDTIMINGS),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_HW_SCRAMBLEMODE   ),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_HW_SCRAMBLESETTINGS),

  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_DBG_OPENGLBUFFERNO    ),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_DBG_CHECKHEAPINTEGRITY),
                                                                
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_HELPERFLAGS           ),

// Keys used by the multiboard wrapper to store WINman2 modes (Binary)
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_MB2D_ET_WINMAN2_DATA_GET0),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_MB2D_ET_WINMAN2_DATA_GET1),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_MB2D_ET_WINMAN2_DATA_GET2),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_MB2D_ET_WINMAN2_DATA_GET3),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_MB2D_ET_WINMAN2_DATA_GET4),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_MB2D_ET_WINMAN2_DATA_GET5),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_MB2D_ET_WINMAN2_DATA_GET6),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_MB2D_ET_WINMAN2_DATA_GET7),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_MB2D_ET_WINMAN2_DATA_GET8),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_MB2D_ET_WINMAN2_DATA_GET9),

// Keys used by the single board driver to store WINman2 modes (Binary)
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ET_WINMAN2_DATA_GET0),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ET_WINMAN2_DATA_GET1),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ET_WINMAN2_DATA_GET2),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ET_WINMAN2_DATA_GET3),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ET_WINMAN2_DATA_GET4),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ET_WINMAN2_DATA_GET5),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ET_WINMAN2_DATA_GET6),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ET_WINMAN2_DATA_GET7),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ET_WINMAN2_DATA_GET8),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_ET_WINMAN2_DATA_GET9),

// tetris tiling configuration
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_HW_TETRIS_TILING),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_HW_TETRIS_MODE  ),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_HW_TETRIS_SHIFT ),

  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_DBG_SURFACENEVERATBOTTOM       ),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_OPENGL_FORCESOFTWARE           ),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_OPENGL_DBGFLAGSOVERRIDE        ),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_OPENGL_DBGMAXLOCALERRORCOUNT   ),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_OPENGL_DBGTRACETOFILE          ),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_OPENGL_USE_DISPLAYDRIVER_BUFFER),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_HW_FORCE_TIMING                ),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_OPENGL_ACQUIREMUTEXFORKICKOFF  ),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_OPENGL_CLIENTCLIPRECTS         ),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_OPENGL_CLIENTRELATIVECLIPRECTS ),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_HW_USE_ELSA_INITCODE           ),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_HW_SHUTDOWN                    ),

  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_OPENGL_NO_ICD_CALLBACK         ),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_OPENGL_OVERLAYBPP              ),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_SZ_OEM_VERSION                    ),
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_GDI_PUNTFLAGS                  ),
                                                                         
  DBG_MAKE_NAMEINDEX(INTERNAL_ET_VALUE_DW_HW_ONDMACRASH                  ),
};


char *szDbg_GetET_VALUE_IDName(DWORD dwID)
{
  return (szDbg_GetDBG_NAMEINDEX(aDbgET_VALUE_IDs, sizeof(aDbgET_VALUE_IDs)/sizeof(aDbgET_VALUE_IDs[0]), (LONG)dwID));
}


#define FL_CAPS_TOGGLE_STEP 3 // needed by vDbg_Printfl2DCapabilities and vDbg_Printfl3DCapabilities
                              // print as much cap strings in one line

/*
** vDbg_PrintflCapabilities
**
** prints line with all set capabilities
**
*/
VOID vDbg_PrintflCapabilities(
  FLONG flDbgFlags, 
  const PCHAR pcString, 
  char *(szFlCaps)(ULONG),  // function pointer to stringizer function (szDbg_GetFL_2DCAPSName)
  FLONG flPrint)
{
  if (0==flPrint)
    DISPDBG((flDbgFlags, "%s: 0 (nothing set)", pcString));
  else
  {
    ULONG ul;
    LONG lToggle = 0;

    DISPDBG((flDbgFlags, "%s: 0x%x", pcString,flPrint));
    for (ul=0; ul<32; ul++)
    {
      if ( bBitsTest(flPrint,1<<ul) )
      {
        if (0==(lToggle % FL_CAPS_TOGGLE_STEP)) // only two in one line
        {
          if (lToggle)
            DISPDBG((flDbgFlags|DBG_LVL_NOPREFIX, " ")); // linefeed
          DISPDBG((flDbgFlags|DBG_LVL_NOLINEFEED, "%s: %s ", pcString, szFlCaps(1<<ul)));
        }
        else
        {
          DISPDBG((flDbgFlags|DBG_LVL_NOPREFIX|DBG_LVL_NOLINEFEED, "%s", szFlCaps(1<<ul)));
        }
        lToggle++;
      }
    }

    DISPDBG((flDbgFlags|DBG_LVL_NOPREFIX, " ")); // linefeed
  }
}


VOID vDbg_PrintET_VERSION(
  FLONG flDbgFlags, 
  PCHAR pcString,   
  const ET_VERSION *pVersion)
{
  if (NULL==pVersion)
    DISPDBG((flDbgFlags, "%s: NULL (ET_VERSION *)", pcString));
  else
  {
    DISPDBG((flDbgFlags, "%s: 0x%p (ET_VERSION *)", pcString, pVersion));
    DISPDBG((flDbgFlags, "%s dwFlags         : 0x%x", pcString, pVersion->dwFlags ));
    DISPDBG((flDbgFlags, "%s dwAlign1        : 0x%x", pcString, pVersion->dwAlign1));
    DISPDBG((flDbgFlags, "%s dwNTMajor       : %d",   pcString, pVersion->dwNTMajor       ));
    DISPDBG((flDbgFlags, "%s dwNTMinor       : %d",   pcString, pVersion->dwNTMinor       ));
    DISPDBG((flDbgFlags, "%s dwNTBuildMajor  : %d",   pcString, pVersion->dwNTBuildMajor  ));
    DISPDBG((flDbgFlags, "%s dwNTBuildMinor  : %d",   pcString, pVersion->dwNTBuildMinor  ));
    DISPDBG((flDbgFlags, "%s dwNTReserved0   : 0x%x", pcString, pVersion->dwNTReserved0   ));
    DISPDBG((flDbgFlags, "%s dwNTReserved1   : 0x%x", pcString, pVersion->dwNTReserved1   ));
    DISPDBG((flDbgFlags, "%s dwNTCapabilities: 0x%x", pcString, pVersion->dwNTCapabilities));
    DISPDBG((flDbgFlags, "%s dwNTVersionStamp: 0x%x", pcString, pVersion->dwNTVersionStamp));
    DISPDBG((flDbgFlags, "%s szVersion       : %s", pcString, pVersion->szVersion));
  }
}


VOID vDbg_PrintET_VERSION_INFO(
  FLONG flDbgFlags, 
  PCHAR pcString,   
  const ET_VERSION_INFO *pVersionInfo)
{
  if (NULL==pVersionInfo)
    DISPDBG((flDbgFlags, "%s: NULL (ET_VERSION_INFO *)", pcString));
  else
  {
    char szTemp[100] = {0};
    char *szPtr;
    if (strlen(pcString)<100)
      strcpy(szTemp,pcString);
    szPtr = szTemp+strlen(szTemp);
    DISPDBG((flDbgFlags, "%s: 0x%p (ET_VERSION_INFO *)", pcString, pVersionInfo));
    DISPDBG((flDbgFlags, "%s dwSize: 0x%x", pcString, pVersionInfo->dwSize));
    if (strlen(pcString)+strlen("Version") < 100)
      strcpy(szPtr,"Version");
    else
      strcpy(szTemp,"Version");
    vDbg_PrintET_VERSION(flDbgFlags, szTemp, &pVersionInfo->Version);
    DISPDBG((flDbgFlags, "%s szRegDriverKey:%s", pcString, pVersionInfo->szRegDriverKey));
  }
}

DBG_NAMEINDEX aDbgDrvINDEX[] =
{
  DBG_MAKE_NAMEINDEX(INDEX_DrvEnablePDEV           ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvCompletePDEV         ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvDisablePDEV          ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvEnableSurface        ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvDisableSurface       ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvAssertMode           ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvResetPDEV            ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvCreateDeviceBitmap   ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvDeleteDeviceBitmap   ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvRealizeBrush         ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvDitherColor          ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvStrokePath           ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvFillPath             ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvStrokeAndFillPath    ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvPaint                ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvBitBlt               ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvCopyBits             ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvStretchBlt           ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvSetPalette           ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvTextOut              ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvEscape               ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvDrawEscape           ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvQueryFont            ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvQueryFontTree        ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvQueryFontData        ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvSetPointerShape      ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvMovePointer          ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvLineTo               ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvSendPage             ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvStartPage            ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvEndDoc               ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvStartDoc             ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvGetGlyphMode         ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvSynchronize          ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvSaveScreenBits       ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvGetModes             ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvFree                 ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvDestroyFont          ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvQueryFontCaps        ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvLoadFontFile         ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvUnloadFontFile       ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvFontManagement       ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvQueryTrueTypeTable   ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvQueryTrueTypeOutline ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvGetTrueTypeFile      ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvQueryFontFile        ),
  #if (_WIN32_WINNT <= 0x400) // not supported under NT5
  DBG_MAKE_NAMEINDEX(INDEX_UNUSED5                 ),
  #endif
  DBG_MAKE_NAMEINDEX(INDEX_DrvQueryAdvanceWidths   ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvSetPixelFormat       ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvDescribePixelFormat  ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvSwapBuffers          ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvStartBanding         ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvNextBand             ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvGetDirectDrawInfo    ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvEnableDirectDraw     ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvDisableDirectDraw    ),
  DBG_MAKE_NAMEINDEX(INDEX_DrvQuerySpoolType       ),
};

char *szDbg_GetDrvINDEX(ULONG ulIndex)
{
  return (szDbg_GetDBG_NAMEINDEX(aDbgDrvINDEX, sizeof(aDbgDrvINDEX)/sizeof(aDbgDrvINDEX[0]), ulIndex));
}

/*
** vDbg_PrintET_VALUE_DWORD
** vDbg_PrintET_VALUE_STRING
**
** Print ELSAESC structures.
**
** FNicklis 11.05.1999: New
*/
VOID vDbg_PrintET_VALUE_DWORD(
  FLONG           flDebugFlags, 
  PCHAR           pcString,   
  ET_VALUE_DWORD *petDWord)
{
  ASSERT(NULL!=pcString);
  if (NULL==petDWord)
  {
    DISPDBG((flDebugFlags, "%s = NULL", pcString));
  }
  else
  {
    DISPDBG((flDebugFlags, "%s.dwSize:0x%08x, .dwID:0x%08x, .dwValue:0x%08x (%d), .dwFlags:0x%08x (%s)", pcString, 
      petDWord->dwSize, petDWord->dwID, petDWord->dwValue, petDWord->dwValue, petDWord->dwFlags, szDbg_GetET_SET_VALUE_RETdwFlags(petDWord->dwFlags)));
  }
}
  
VOID vDbg_PrintET_VALUE_STRING(
  FLONG            flDebugFlags, 
  PCHAR            pcString,   
  ET_VALUE_STRING *petString)
{
  ASSERT(NULL!=pcString);
  if (NULL==petString)
  {
    DISPDBG((flDebugFlags, "%s = NULL", pcString));
  }
  else
  {
    DISPDBG((flDebugFlags, "%s.dwSize:0x%08x, .dwID:0x%08x, .szValue:%s, .dwFlags:0x%08x (%s)", pcString, 
      petString->dwSize, petString->dwID, petString->szValue, petString->dwFlags, szDbg_GetET_SET_VALUE_RETdwFlags(petString->dwFlags)));
  }
}

#endif //DBG
#endif //USE_WKS_ELSA_TOOLS
// End of wkselsatoolsdebug.c
