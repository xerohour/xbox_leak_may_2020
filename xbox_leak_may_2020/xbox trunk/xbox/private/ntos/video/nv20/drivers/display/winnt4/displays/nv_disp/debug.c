//*****************************Module*Header************************************
//
// Module Name: debug.c
//
// Debug helper routines.
//
// Copyright (c) 1992-1996 Microsoft Corporation
//
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/

#include "precomp.h"
#include "driver.h"

#if DBG
    #ifndef _DEBUG_H
    #error debug.c should see debug.h, please include!
    #endif

////////////////////////////////////////////////////////////////////////////
// DEBUGGING INITIALIZATION CODE
//
// When you're bringing up your display for the first time, you can
// recompile with 'DebugLevel' set to 100.  That will cause absolutely
// all DISPDBG messages to be displayed on the kernel debugger (this
// is known as the "PrintF Approach to Debugging" and is about the only
// viable method for debugging driver initialization code).

LONG DebugLevel = 0;            // Set to '100' to debug initialization code
                                //   (the default is '0')

BOOL    g_bNvbreak = FALSE;     // By default, we don't want to assert.

////////////////////////////////////////////////////////////////////////////

LONG gcFifo = 0;                // Number of currently free FIFO entries

BOOL gbCrtcCriticalSection = FALSE;
                                // Have we acquired the CRTC register
                                //   critical section?

#define LARGE_LOOP_COUNT  10000000

////////////////////////////////////////////////////////////////////////////
// Miscellaneous Driver Debug Routines
////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
 *
 *   Routine Description:
 *
 *      This function is variable-argument, level-sensitive debug print
 *      routine.
 *      If the specified debug level for the print statement is lower or equal
 *      to the current debug level, the message will be printed.
 *
 *   Arguments:
 *
 *      DebugPrintLevel - Specifies at which debugging level the string should
 *          be printed
 *
 *      DebugMessage - Variable argument ascii c string
 *
 *   Return Value:
 *
 *      None.
 *
 ***************************************************************************/

VOID
DebugPrint(
    LONG  DebugPrintLevel,
    PCHAR DebugMessage,
    ...
    )
{
    va_list ap;

    va_start(ap, DebugMessage);

    if (DebugPrintLevel <= DebugLevel)
    {
#ifdef _WIN32_WINNT         // NT 4.x
        EngDebugPrint(STANDARD_DEBUG_PREFIX, DebugMessage, ap);
        EngDebugPrint("", "\n", ap);
#else
        char buffer[128];

        vsprintf(buffer, DebugMessage, ap);
        OutputDebugStringA(buffer);
        OutputDebugStringA("\n");
#endif                      // NT 4.x
    }

    va_end(ap);

} // DebugPrint()



/******************************Public*Routine******************************\
* VOID vAcquireCrtc()
* VOID vReleaseCrtc()
*
* Debug thunks for grabbing the CRTC register critical section.
*
\**************************************************************************/

VOID vAcquireCrtc(PDEV* ppdev)
{
    EngAcquireSemaphore(ppdev->csCrtc);

    if (gbCrtcCriticalSection)
        RIP("Had already acquired Critical Section");
    gbCrtcCriticalSection = TRUE;
}

VOID vReleaseCrtc(PDEV* ppdev)
{
    if (!gbCrtcCriticalSection)
        RIP("Hadn't yet acquired Critical Section");
    gbCrtcCriticalSection = FALSE;
    EngReleaseSemaphore(ppdev->csCrtc);
}


//*************************************************************************
// szDbg_GetDBG_NAMEINDEX
// 
// Helper function for DBG_NAMEINDEX to retrieve the string matching 
// ulIndex out of array pNameIndex. 
//
// NOTE: debug.h for more information
//*************************************************************************
char *szDbg_GetDBG_NAMEINDEX(
    IN const DBG_NAMEINDEX *pNameIndex, // pointer to array with lEntries members
    IN       LONG           lEntries,   // number of valid array elements
    IN       ULONG          ulIndex)    // index to find within array
{
    char *szRet=NULL;

    while (lEntries)
    {
        if (pNameIndex->ulIndex == ulIndex)
        { 
            // found index, return string
            szRet = pNameIndex->szName;
            break;
        }
        pNameIndex++;
        lEntries--;
    }

    if (szRet == NULL)
    {
        // entry not found, return default string
        szRet = "unknown";
    }

    return (szRet);
}


//*************************************************************************
// vDbg_PrintDBG_NAMEINDEX
// 
// General debug print for DBG_NAMEINDEX member with ulIndex, using 
// debuglevel and leading string.
//*************************************************************************
VOID vDbg_PrintDBG_NAMEINDEX(
    LONG         DebugPrintLevel,    // debug level
    const PCHAR  pcString,           // any leading string
    char        *(szFlCaps)(ULONG),  // function pointer to stringizer function
    ULONG        ulIndex)            // index to find and to which the matching name has to be printed
{
    DISPDBG((DebugPrintLevel, "%s 0x%08x %s", pcString, ulIndex, szFlCaps(ulIndex) ));
}


//*************************************************************************
// vDbg_PrintDBG_NAMEINDEXfl
// 
// Same as vDbg_PrintDBG_NAMEINDEX but interpreting fl as bitfield and 
// calling stringizer function for every bit.
//*************************************************************************
VOID vDbg_PrintDBG_NAMEINDEXfl(
  LONG DebugPrintLevel, 
  const PCHAR pcString, 
  char *(szFlCaps)(ULONG),  // function pointer to stringizer function
  FLONG fl)
{
    ULONG ul;

    DISPDBG((DebugPrintLevel, "%s: 0x%8x ->", pcString,fl));
    for (ul=0; ul<32; ul++)
    {
        ULONG ulBit = 1<<ul;
        if (fl & ulBit)
        {
            DISPDBG(( DebugPrintLevel, "  0x%8x %s", ulBit, szFlCaps(ulBit) ));
        }
    }
}


//*************************************************************************
// DrvEscape-iEsc stuff:                        
//                                                        
// szDbg_iEscGet                                        
//   return string matching the iEsc
// aDbg_iEsc                                             
//   array defining all known escapes                            
//*************************************************************************
DBG_NAMEINDEX aDbg_iEsc[] =
{
#if _WIN32_WINNT >= 0x0500
    {3076, "MCDFUNCS"},
#else
    {3076, "RXFUNCS" },
#endif
    DBG_MAKE_NAMEINDEX(QUERYESCSUPPORT          ),
    DBG_MAKE_NAMEINDEX(OPENGL_CMD               ),
    DBG_MAKE_NAMEINDEX(OPENGL_GETINFO           ),
    DBG_MAKE_NAMEINDEX(NV_OPENGL_SET_ENABLE     ),
    DBG_MAKE_NAMEINDEX(ESC_ALLOC_CONTEXT_DMA    ),
    DBG_MAKE_NAMEINDEX(ESC_FREE_CONTEXT_DMA     ),
    DBG_MAKE_NAMEINDEX(ESC_ALLOC_CHANNEL_DMA    ),
    DBG_MAKE_NAMEINDEX(ESC_FREE_CHANNEL_DMA     ),
    DBG_MAKE_NAMEINDEX(ESC_DMA_FLOW_CONTROL     ),
    DBG_MAKE_NAMEINDEX(ESC_SET_GAMMA_RAMP       ),
    DBG_MAKE_NAMEINDEX(ESC_GET_GAMMA_RAMP       ),
    DBG_MAKE_NAMEINDEX(ESC_SET_GAMMA_RAMP_MULTI ),
    DBG_MAKE_NAMEINDEX(ESC_GET_GAMMA_RAMP_MULTI ),
    DBG_MAKE_NAMEINDEX(ESC_GET_PIXEL_FORMAT     ),

    DBG_MAKE_NAMEINDEX(NV_ESC_RM_BASE                  ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_OPEN                  ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_CLOSE                 ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_ALLOC_ROOT            ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_ALLOC_DEVICE          ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_ALLOC_CONTEXT_DMA     ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_ALLOC_CHANNEL_PIO     ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_ALLOC_CHANNEL_DMA     ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_ALLOC_MEMORY          ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_ALLOC_OBJECT          ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_FREE                  ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_DMA_PUSH_INFO         ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_ALLOC                 ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_CONFIG_VERSION        ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_CONFIG_GET            ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_CONFIG_SET            ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_CONFIG_UPDATE         ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_ARCH_HEAP             ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_DEBUG_CONTROL         ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_CONFIG_GET_EX         ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_CONFIG_SET_EX         ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_I2C_ACCESS            ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_POWER_MANAGEMENT      ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_INTERRUPT             ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_OS_CONFIG_GET         ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_OS_CONFIG_SET         ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_OS_CONFIG_GET_EX      ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_OS_CONFIG_SET_EX      ),
    DBG_MAKE_NAMEINDEX(NV_ESC_RM_NVWATCH               ),

    DBG_MAKE_NAMEINDEX(WNDOBJ_SETUP),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_DMA_PUSH_GO),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_ESCAPE),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_MEM_TO_SCREEN_SRCCOPY       ),
    DBG_MAKE_NAMEINDEX(ESC_NV_QUERYSET_REGISTRY_KEY              ),
    DBG_MAKE_NAMEINDEX(ESC_NV_MAP_USER	),
    DBG_MAKE_NAMEINDEX(ESC_NV_UNMAP_USER),
    DBG_MAKE_NAMEINDEX(ESC_NV_QUERY_PCI_SLOT                  ),
    DBG_MAKE_NAMEINDEX(ESC_NV_QUERY_HEAD_REFRESH_RATE         ),
    DBG_MAKE_NAMEINDEX(NV_ESC_PRIMARY_INFO),
    DBG_MAKE_NAMEINDEX(ESC_NV_DESKMGR_ZOOMBLIT),
    DBG_MAKE_NAMEINDEX(ESC_NV_WKS_APP_SUPPORT),
};

char *szDbg_iEscGet(ULONG iEsc)
{
  return (DBG_GETDBG_NAMEINDEX(aDbg_iEsc, iEsc));
}

DBG_NAMEINDEX aDbg_NV_OPENGL_ESCAPE[] =
{
    //DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_CREATE_DRAWABLE             ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_DESTROY_DRAWABLE            ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_START_STATE_CLIENT_TRACKING ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_STOP_STATE_CLIENT_TRACKING  ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_DMA_PUSH_GO_CMD             ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_MEM_TO_SCREEN_SRCCOPY       ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_SCREEN_TO_SCREEN_SRCCOPY    ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_SUPPORT_ENABLED             ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_ALLOC_SHARED_MEMORY         ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_FREE_SHARED_MEMORY          ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_GET_DISPLAY_PITCH           ),
    DBG_MAKE_NAMEINDEX(ESC_NV4_OPENGL_SWAP_BUFFERS               ),
    DBG_MAKE_NAMEINDEX(ESC_NV4_OPENGL_FLUSH                      ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_PURGE_DEVICE_BITMAP         ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_FLUSH                       ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_SWAP_BUFFERS                ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_COMMAND                     ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_CPUBLIT                     ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_INIT_STATE                  ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_ALLOC_UNIFIED_SURFACES      ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_FREE_UNIFIED_SURFACES       ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_GET_SURFACE_INFO            ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_DID_STATUS_CHANGE           ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_GET_CLIP_LIST_COUNT         ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_GET_CLIP_LIST               ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_SUPPORT                     ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_REGISTER_RM_CLIENT          ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_UNREGISTER_RM_CLIENT        ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_SET_DAC_BASE                ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_SYNC_CHANNEL                ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_SET_LAYER_PALETTE           ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_GET_LAYER_PALETTE           ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_OVERLAY_MERGEBLIT           ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_PFD_CHECK                   ),
    DBG_MAKE_NAMEINDEX(ESC_NV_OPENGL_SHARED_LIBRARY_SIZES        ),
};

char *szDbg_NV_OPENGL_ESCAPE(ULONG iEsc)
{
  return (DBG_GETDBG_NAMEINDEX(aDbg_NV_OPENGL_ESCAPE, iEsc));
}


//
// SURFOBJ stuff
//
DBG_NAMEINDEX aDbg_SURFOBJiBitmapFormat[] =
{
    DBG_MAKE_NAMEINDEX(BMF_1BPP ),
    DBG_MAKE_NAMEINDEX(BMF_4BPP ),
    DBG_MAKE_NAMEINDEX(BMF_8BPP ),
    DBG_MAKE_NAMEINDEX(BMF_16BPP),
    DBG_MAKE_NAMEINDEX(BMF_24BPP),
    DBG_MAKE_NAMEINDEX(BMF_32BPP),
    DBG_MAKE_NAMEINDEX(BMF_4RLE ),
    DBG_MAKE_NAMEINDEX(BMF_8RLE ),
#if _WIN32_WINNT >= 0x0500
    DBG_MAKE_NAMEINDEX(BMF_JPEG ),
    DBG_MAKE_NAMEINDEX(BMF_PNG  ),
#endif
};

char *szDbg_SURFOBJiBitmapFormat(ULONG iBitmapFormat)
{
  return (DBG_GETDBG_NAMEINDEX(aDbg_SURFOBJiBitmapFormat, iBitmapFormat));
}

#define STYPE_DEVBITMAP_DIB 'derf'

DBG_NAMEINDEX aDbg_SURFOBJiType[] =
{
    DBG_MAKE_NAMEINDEX(STYPE_BITMAP   ),
    DBG_MAKE_NAMEINDEX(STYPE_DEVICE   ),
    DBG_MAKE_NAMEINDEX(STYPE_DEVBITMAP),
    // the following doesn't exist!
    DBG_MAKE_NAMEINDEX(STYPE_DEVBITMAP_DIB),
};

char *szDbg_SURFOBJiType(USHORT iType)
{
  return (DBG_GETDBG_NAMEINDEX(aDbg_SURFOBJiType, (ULONG)iType));
}

char *szDbg_SURFOBJ_surf(SURFOBJ *pso)
{
    if (pso)
    {
        if(IS_DIB_DEVICEBITMAP(pso))
        {
            ASSERT(STYPE_DEVBITMAP==pso->iType);
            return(szDbg_SURFOBJiType((USHORT)STYPE_DEVBITMAP_DIB));
        }
        else
            return(szDbg_SURFOBJiType(pso->iType));
    }
    else
    {
        return "";
    }
}

//
// CLIPOBJ stuff
// 
DBG_NAMEINDEX aDbg_CLIPOBJiDComplexity[] =
{
    DBG_MAKE_NAMEINDEX(DC_TRIVIAL),
    DBG_MAKE_NAMEINDEX(DC_RECT   ),
    DBG_MAKE_NAMEINDEX(DC_COMPLEX),
};

char *szDbg_CLIPOBJiDComplexity(BYTE iDComplexity)
{
  return (DBG_GETDBG_NAMEINDEX(aDbg_CLIPOBJiDComplexity, (ULONG)iDComplexity));
}

//
// XLATEOBJ stuf
//
DBG_NAMEINDEX aDbg_XLATEOBJflXlate[] =
{
    DBG_MAKE_NAMEINDEX(XO_TRIVIAL   ),
    DBG_MAKE_NAMEINDEX(XO_TABLE     ),
    DBG_MAKE_NAMEINDEX(XO_TO_MONO   ),
#if _WIN32_WINNT >= 0x0500
    DBG_MAKE_NAMEINDEX(XO_FROM_CMYK ),
    DBG_MAKE_NAMEINDEX(XO_DEVICE_ICM),
    DBG_MAKE_NAMEINDEX(XO_HOST_ICM  ),
#endif //_WIN32_WINNT >= 0x0500
};

char *szDbg_XLATEOBJflXlate(FLONG flXlate)
{
  return (DBG_GETDBG_NAMEINDEX(aDbg_XLATEOBJflXlate, (ULONG)flXlate));
}





////////////////////////////////////////////////////////////////////////////

#endif // DBG
