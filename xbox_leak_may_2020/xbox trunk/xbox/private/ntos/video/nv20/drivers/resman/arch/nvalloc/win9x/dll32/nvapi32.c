 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1997 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/******************* Operating System Interface Routines *******************\
*                                                                           *
* Module: NVAPI.C                                                           *
*   This is the Windows half of the Resource Manager.  System clients call  *
*   the Resource Manager through these APIS.  The VxD calls into this       *
*   to post a message to the Windows Resource Manager EXE.                  *
*                                                                           *
\***************************************************************************/

#include <windows.h>
//
// NvClient API to Resource Manager.
//
#include <nvwin32.h>
#include <nvrmwin.h>
#include <nv_ref.h>
//#include <nvcm.h>

//
// pragmas
//
#pragma warning(disable : 4035)

//---------------------------------------------------------------------------
//
//  Defines.
//
//---------------------------------------------------------------------------
#define NV_CFG_VBLANK_STATUS        500
#define NV_CFG_GE_STATUS            501
#define NV_CFG_CURRENT_SCANLINE     502
#define NUM_CLIENTS                 16
#define NUM_FIFOS                   32

// need these defines for the inline assembly
#define RegEDI	16
#define RegDS	28
#define RegCS	32

//---------------------------------------------------------------------------
//
//  Typedefs.
//
//---------------------------------------------------------------------------
typedef struct _def_rmParams FAR *LPRMPARAMS;

//---------------------------------------------------------------------------
//
//  Prototypes.
//
//---------------------------------------------------------------------------

//
// Prototype of dispatch routine.
//
DWORD FAR PASCAL _Nv32Dispatch(LPRMPARAMS, DWORD);
BOOL _stdcall THK_ThunkConnect32(LPSTR, LPSTR, HINSTANCE, DWORD);
DWORD PASCAL NvSysDispatch(LPRMPARAMS, DWORD);

//---------------------------------------------------------------------------
//
//  Data.
//
//---------------------------------------------------------------------------
DWORD dll32_DeviceInstance=0;
// Linear address of the chip
DWORD *pNv = NULL;

//---------------------------------------------------------------------------
//
//  Nv non-client entrypoints.
//
//---------------------------------------------------------------------------

//
// DLL initialization and termination routine.
//
BOOL APIENTRY DllMain
(
    HANDLE hInstance,
    DWORD  dwReason,
    LPSTR  lpszCmdLine
)
{
    static INT AttachCount = 0;

    if (!(THK_ThunkConnect32("NVARCH16.DLL", "NVARCH32.DLL", hInstance, dwReason)))
        return (FALSE);

    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
            ++AttachCount;
            break;
        case DLL_PROCESS_DETACH:
            //_Nv32Dispatch(&NvParams, 3);  // Don't detach since OGL is actually multiple links
            --AttachCount;
            break;
    }
    return (TRUE);
}
//
// Dispatcher to Resource Manager.
//
DWORD PASCAL _Nv32Dispatch
(
    LPRMPARAMS lpParams,
    DWORD      Function
)
{
    //
    // Copy the flat selectors to the parameter list for the 16 bit
    // dispatcher to use.
    //
    _asm mov edi, lpParams
    _asm xor eax, eax
    _asm mov ax, ds
    _asm mov [edi].RegDS, eax
    _asm mov ax, cs
    _asm mov [edi].RegCS, eax
    _asm mov eax, dll32_DeviceInstance
    _asm mov [edi].RegEDI, eax 
    NvSysDispatch(lpParams, Function);
}

//---------------------------------------------------------------------------
//
//  Nv client entrypoints.
//
//---------------------------------------------------------------------------
int __stdcall Nv32SetDeviceInstance(DWORD DeviceHandle)
{
  dll32_DeviceInstance = DeviceHandle;
  return(0);
}

int __stdcall NvGetFlatDataSelector
(
    void
)
{
    DWORD Selector;

    _asm xor eax, eax
    _asm mov ax, ds
    _asm mov Selector, eax
    return (Selector);
}
int __stdcall NvGetFlatCodeSelector
(
    void
)
{
    DWORD Selector;

    _asm xor eax, eax
    _asm mov ax, cs
    _asm mov Selector, eax
    return (Selector);
}

//---------------------------------------------------------------------------
//
//  Nv configuration manager entrypoints.
//
//---------------------------------------------------------------------------

int __stdcall NvConfigVersion
(
    void
)
{
    RMPARAMS NvParams;

    _Nv32Dispatch(&NvParams, NV_FUNCTION_NVCONFIGVERSION);
    return (NvParams.RegEAX);
}
int __stdcall NvConfigGet
(
    DWORD Index,
    DWORD DeviceHandle
)
{
    RMPARAMS NvParams;

    //
    // If this is a different device than the last call, flush
    // the chip pointer
    //
    if (DeviceHandle != dll32_DeviceInstance)
        pNv = NULL;
        
    Nv32SetDeviceInstance(DeviceHandle);

    //
    // Watch for indices that we want to handle right here in 32bit.  Thunking down
    // to the 16bit DLL takes too long for some DCT flipping tests that are very
    // timing sensitive.
    //
    switch (Index)
    {
        case NV_CFG_VBLANK_STATUS:
            if (pNv == NULL)
            {
                //
                // Get a pointer to this chip
                //
				NvParams.RegESI = DeviceHandle-1;
                _Nv32Dispatch(&NvParams, NV_FUNCTION_NVSYSGETHWPTRS);
                pNv = (DWORD *)NvParams.RegEAX;
            }
            //
            // Grab the VBLANK status bit
            //
            NvParams.RegEAX = (((U032)*(pNv + (NV_PCRTC_RASTER/4))) & 0x00010000) >> 16;
                    
            break;
            
        case NV_CFG_CURRENT_SCANLINE:
            if (pNv == NULL)
            {
                //
                // Get a pointer to this chip
                //
				NvParams.RegESI = DeviceHandle-1;
                _Nv32Dispatch(&NvParams, NV_FUNCTION_NVSYSGETHWPTRS);
                pNv = (DWORD *)NvParams.RegEAX;
            }
            //
            // Grab the current scanline value
            //
            NvParams.RegEAX = ((U032)*(pNv + (NV_PCRTC_RASTER/4))) & 0x7FF;
            break;
            
        default:
    
            NvParams.RegEAX = Index;
			NvParams.RegESI = DeviceHandle-1;
            _Nv32Dispatch(&NvParams, NV_FUNCTION_NVCONFIGGET);
            break;
    }
            
    return (NvParams.RegEAX);
}
int __stdcall NvConfigSet
(
    DWORD Index,
    DWORD Value,
    DWORD DeviceHandle
)
{
    RMPARAMS NvParams;
    
    Nv32SetDeviceInstance(DeviceHandle);

    NvParams.RegEAX = Index;
    NvParams.RegEBX = Value;
	NvParams.RegESI = DeviceHandle-1;
    _Nv32Dispatch(&NvParams, NV_FUNCTION_NVCONFIGSET);
    return (NvParams.RegEAX);
}
int __stdcall NvConfigUpdate
(
    DWORD Flag,
    DWORD DeviceHandle
)
{
    RMPARAMS NvParams;

    Nv32SetDeviceInstance(DeviceHandle);

    NvParams.RegEAX = Flag;
	NvParams.RegESI = DeviceHandle-1;
    _Nv32Dispatch(&NvParams, NV_FUNCTION_NVCONFIGUPDATE);
    return (NvParams.RegEAX);
}

//---------------------------------------------------------------------------
//
//  Nv device manager entrypoints.
//
//---------------------------------------------------------------------------

int __stdcall NvGetHardwarePointers
(
    DWORD *pNv1,
    DWORD *pAlternate,
    DWORD *pFramebuffer,
    DWORD *pTimer
)
{
    RMPARAMS NvParams;
    
    //
    // Get all the hardware pointers.
    //
    _Nv32Dispatch(&NvParams, NV_FUNCTION_NVSYSGETHWPTRS);
    if (pNv1)         *pNv1         = NvParams.RegEAX;
    if (pAlternate)   *pAlternate   = NvParams.RegEBX;
    if (pFramebuffer) *pFramebuffer = NvParams.RegECX;
    if (pTimer)       *pTimer       = NvParams.RegEDX;
}
int __stdcall NvDeviceBaseGet
(
    DWORD Index,
    DWORD DeviceHandle
)
{
    RMPARAMS NvParams;

    Nv32SetDeviceInstance(DeviceHandle);

    NvParams.RegEAX = Index;
	NvParams.RegESI = DeviceHandle-1;
    _Nv32Dispatch(&NvParams, NV_FUNCTION_NVDEVICEBASEGET);
    return (NvParams.RegEAX);
}
int __stdcall NvDeviceLimitGet
(
    DWORD Index,
    DWORD DeviceHandle
)
{
    RMPARAMS NvParams;

    Nv32SetDeviceInstance(DeviceHandle);
    
    NvParams.RegEAX = Index;
	NvParams.RegESI = DeviceHandle-1;
    _Nv32Dispatch(&NvParams, NV_FUNCTION_NVDEVICELIMITGET);
    return (NvParams.RegEAX);
}
int __stdcall NvDeviceSelectorGet
(
    DWORD Index
)
{
    RMPARAMS NvParams;

    NvParams.RegEAX = Index;
    _Nv32Dispatch(&NvParams, NV_FUNCTION_NVDEVICESELECTORGET);
    return (NvParams.RegEAX);
}

//---------------------------------------------------------------------------
//
//  Nv system DLL entrypoints.
//
//---------------------------------------------------------------------------

void __stdcall NvSysShowHideCursor
(
    int HideShow
)
{
    RMPARAMS NvParams;

    NvParams.RegEAX = HideShow;
    _Nv32Dispatch(&NvParams, NV_FUNCTION_NVSYSSHOWHIDECRSR);
}

int __stdcall NvDmaPushOperation
(
    U032  flags,
    VOID  *pDmaPushInfo,
    DWORD DeviceHandle
)
{
    RMPARAMS NvParams;
    
    Nv32SetDeviceInstance(DeviceHandle);

	NvParams.RegESI = DeviceHandle-1;
    NvParams.RegEAX = NVRM_API_DMAPUSH_OPERATION;
    NvParams.RegEBX = flags;
    NvParams.RegECX = (U032) pDmaPushInfo;
    NvParams.RegES  = NvGetFlatDataSelector();

    _Nv32Dispatch(&NvParams, 0);

    return 0;
}


